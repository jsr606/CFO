// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
//#define MIDI
#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>

const int pushButton=6;
const int slideSwitch=4;
const int LED=9;
const int bodySwitch1 = A6;
const int bodySwitch2 = A3;
const int bodySwitch3 = A7;

const int lightSensor = A1;

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

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  Music.setWaveform1(SAW);
  Music.setEnv1Sustain(127);
//  Music.setEnv2Sustain(127);
  Music.setCutoff(45*256);
  Music.setGain2(0.0);
  Music.setGain3(0.0);

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

int minRead = 1023, maxRead = 0;

void loop() {

  usbMIDI.read();
  
  boolean slide = digitalRead(slideSwitch);
  
  int lightLevel = analogRead(lightSensor);
  minRead = min(lightLevel, minRead);
  maxRead = max(lightLevel, maxRead);
  
  timeDelay = map(lightLevel, minRead, maxRead, 500, 700);
  
  if (slide) {
    timeNow = millis();
    if((timeNow - lastTime) > timeDelay) {
      Music.noteOn(48);
      lastTime = timeNow;
    }
  } else {
    Music.noteOff();
  }

  int bs1 = analogRead(bodySwitch1);
  int bs2 = analogRead(bodySwitch2);
  int bs3 = analogRead(bodySwitch3);
  
  /*
  Serial.print(bs1);
  Serial.print("\t");
  Serial.print(bs2);
  Serial.print("\t");
  Serial.print(bs3);
  Serial.println("\t");
  */
  
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
  
  if (bs2 > bodyThreshold || bs3 > bodyThreshold) {
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
   
}
