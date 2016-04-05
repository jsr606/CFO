// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
//#define MIDI
#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>
#include <Sequencer.h>

const int pushButton=6;
const int slideSwitch=4;
const int LED=9;
const int bodySwitch1 = A6;
const int bodySwitch2 = A3;
const int bodySwitch3 = A7;
const int microphone = A8;

long timeNow = 0;
long lastTime = 0;
long timeDelay = 500;

int currentPreset = 16;
boolean lastButtonState = false;
int bodyThreshold = 100;

boolean bs1Pushed = false, bs2Pushed = false, bs3Pushed = false;

unsigned long lastPreset = millis();
unsigned long lastSave = millis();
int saveDelay = 1000;

int pushDelay = 250;

uint8_t myInstrument[128];

int userPreset = 0;

// sequencer
int seq0_indx = 0;
const int seq0_len = 4;
const int seq0_notes[] = {12, 12, 12, 12};
int seq1_indx = 0;
const int seq1_len = 12;
const int seq1_notes[] = {36, 12, 24, 12, 0, 12, 48, 36, 12, 24, 24, 36};
int tick = 0;
const float p = 0.75;
int s1, s2, s3;

// flute
float lastVol;
float easing = 0.01;
const int scale[] = {0,3,6,7,9,10,12+0,12+3,12+6,12+7,12+9,12+10};

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();

  // init sequencer
  Sequencer.init();

  //Sequencer.newSequence(bpm, callback function, subdiver)
  s1 = Sequencer.newSequence(120, &s1cb, 8 * p);
  s2 = Sequencer.newSequence(120, &s2cb, 16 * p);  
  s3 = Sequencer.newSequence(120, &s3cb, 8 * p);
  
  Sequencer.startSequence(s1);
  Sequencer.startSequence(s3);  
  
  // init MIDI

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);  

  // internal pullups
  pinMode(slideSwitch, INPUT_PULLUP);  
  pinMode(pushButton, INPUT_PULLUP);
  
  pinMode(LED, OUTPUT);

  // set internal pulldown resistors on bodyswitches
  *portConfigRegister(bodySwitch1) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  *portConfigRegister(bodySwitch2) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  *portConfigRegister(bodySwitch3) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  
}

void loop() {

  // check for incoming MIDI
  usbMIDI.read();
  
  boolean slide = digitalRead(slideSwitch);
  
  if (slide) {
    // sequencer mode
    Sequencer.update();
    soundPresets();
  } else {
    // chord/drone mode
    chordMode();    
  }
   
}

void chordMode() {
  Music.disableEnvelope1();
  Music.disableEnvelope2();

  int bs1 = analogRead(bodySwitch1);
  int bs2 = analogRead(bodySwitch2);
  int bs3 = analogRead(bodySwitch3);
  
  int tone1 = map(bs1,0,1023,0,12);
  int tone2 = map(bs2,0,1023,0,12);
  int tone3 = map(bs3,0,1023,0,12);
  
  
  
  Music.setSemitone1(scale[tone1]-12);
  Music.setSemitone2(scale[tone2]-12);
  Music.setSemitone3(scale[tone3]-12);
  
  
  int mic = sampleMic(20);
  //Serial.print("mic level ");
  //Serial.print(mic);
  
  int micPower = 30;
  
  float mapped = map(mic,15,800,0,micPower);
  mapped = constrain(mapped,0,micPower)/micPower;
  
  float deltaVol = mapped - lastVol;
  float newVol = lastVol + deltaVol * easing;
  
  analogWrite(LED, int(newVol*255));
  
  //Serial.print("\t set gain to ");
  //Serial.println(mapped);
  
  Music.setGain(newVol);
  
  lastVol = newVol;

}

void soundPresets() {
  int bs1 = analogRead(bodySwitch1);
  int bs2 = analogRead(bodySwitch2);
  int bs3 = analogRead(bodySwitch3);
  
  if (bs2 > bodyThreshold || bs3 > bodyThreshold) {
    if (lastPreset+pushDelay < millis()) {
      
      digitalWrite(LED, HIGH);
      
      int randomization = map(bs1,0,1023,0,25);
      
      if (bs2 > bodyThreshold && bs3 > bodyThreshold) {
        // both bs2 + bs3 = stay on this preset
        // check if bs1 is pressed
        if (bs1 > bodyThreshold) {
          Music.getRandomizedPreset(currentPreset,randomization);
        }
      } else if (bs2 > bodyThreshold) {
        // bs2 = one lower
        currentPreset--;
        if (currentPreset < 0) currentPreset = 63;
        Music.getRandomizedPreset(currentPreset,randomization);
      } else if (bs3 > bodyThreshold) {
        // bs3 = one higher
        currentPreset++;
        if (currentPreset > 63) currentPreset = 0;
        Music.getRandomizedPreset(currentPreset,randomization);
      }
      lastPreset = millis();
    } else {
      digitalWrite(LED, LOW);
      lastButtonState = false;
    }
  }

  // save current sound if button pushed
  boolean push = !digitalRead(pushButton);
  if (push) {
    if (lastButtonState == false && lastSave + saveDelay < millis()) {
      // save user preset
      // NB: will delete previously saved presets!
      Music.savePreset(userPreset);
      userPreset++;
      userPreset = userPreset % 16;
      lastButtonState = true;
      lastSave = millis();
    } 
  } else {
    lastButtonState = false;
  }

}


// callback for sequencer #1
void s1cb() {
  Music.enableEnvelope1();
  //Music.setWaveform(SINE);
  Music.setDetune(0.01);
  Music.noteOn(37); // baseline
  if(tick++ == 8) {
    // start sequene #2 on beat
    Sequencer.startSequence(s2);
    tick = 0;
  }
}

// callback for sequencer #2
void s2cb() {
  Music.enableEnvelope1();
  //Music.setWaveform(SINE);
  Music.setDetune(0);
  Music.noteOn(seq0_notes[seq0_indx++]  + 24, 127);
  if(seq0_indx > seq0_len) {
    //if done with the sequence then stop it
    Sequencer.stopSequence(s2);
    seq0_indx = 0;
  }
}

// callback for sequencer #3
void s3cb() {
  // this is simple tone / no envelop
  Music.disableEnvelope1();
  //Music.setWaveform(SINE);
  Music.setDetune(0.03);
  Music.noteOn(seq1_notes[seq1_indx++]  + 12, 127);
  if(seq1_indx > seq1_len) {
    seq1_indx = 0;
  }
}

int sampleMic(int samples) {
  
  // we are feeding the mic 5V, but sampling it to 3,3V
  // audio waves are centered around 2,5V (not 3,3V/2)
  // this means readings are centered around 1023*2,5/3,3=775, not 512 which would be the normal
  
  int totalVol = 0;
  int samplesDone = 0;
  while (samplesDone < samples) {
    
    int thisReading = analogRead(microphone)-775;
    
    if (thisReading < 0) {
      totalVol = totalVol + abs(thisReading);
      samplesDone++;
    }
    delayMicroseconds(5);
  }
  return (totalVol/samples);
}
