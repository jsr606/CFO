  // The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
#include <SoftPWM.h>

#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

const int seqLed [] = {2,3,4,5,6,7,8,9};
int seqLedVal [] = {0,0,0,0,0,0,0,0};
const int statusLed1 = 10, statusLed2 = 11;
const int button1 = 12, button2 = 13;
const int pot1 = A0, pot2 = A1;
const int bodySwitch [] = {A2,A3,A4,A5,A6,A7,A8,A9};

unsigned long lastPrint = millis();

boolean sequencerRunning = true;

float maxBodyReading = 0;
float maxBodyFadeout = 0.9999;

float averageNoise = 0;

int inputFreq = 10;
unsigned long lastInput = millis();

int seqStep = 0;
int seqLength = 7;
int seqNote[] = {-1,-1,0,-1,12,-1,-1,-1,-1,-1,0,-1,24,-1,-1,-1,-1,-1,0,-1,36,-1,-1,-1,-1,-1,0,-1,0,-1,-1,-1};
int activeSeq = 0, seqStart = 0, seqEnd = 7;
unsigned long lastStep = millis();
int stepTime = 200;
int bassNote = 36;

const int scale[3][7] = {
  {0,2,4,5,7,9,11}, // major
  {0,2,3,5,6,7,10}, // blues
  {0,3,4,7,9,10,-1} // rock
};


int bodySwitchVal [] = {0,0,0,0,0,0,0,0};

int mode = 0;
int preset = 16;

boolean buttonPress1 = false, buttonPress2 = false, buttonRelease1 = false, buttonRelease2 = false, doublePress = false;

int potVal1 = 0, potVal2 = 0;
boolean potMoved1 = false, potMoved2 = false;
int potNoise = 1;

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  Music.getPreset(16);
  Music.setEnv1Decay(36);
  Music.setEnv1Sustain(0);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  
  analogReadAveraging(32);
  
  for (int i = 0; i<8; i++) {
    pinMode(seqLed[i], OUTPUT);
    SoftPWMSet(seqLed[i], 0);
    SoftPWMSetFadeTime(seqLed[i], 0, 0);
    averageNoise += analogRead(bodySwitch[i]);
  }
  
  averageNoise = averageNoise/8 + 3;
  
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  
  SoftPWMBegin();
  
  startupAnimation();
  
}

void loop() {
  
  // check for incoming USB MIDI messages
  usbMIDI.read();

  if (lastInput + inputFreq < millis()) {
    
    // check user input
    readPots();
    readBodyswitches(); 
    readButtons();
    
    // general navigation
    if (buttonPress2 && !doublePress) {
      changeMode();
      changePreset();
    } else {
      updateLEDs();
    }
    
    lastInput = millis();
  
  }
  
  
  
  switch (mode) {
    case 0:
      // sequencer mode 0, active sequence 0
      activeSeq = 0;
      if (sequencerRunning) updateSequence();
      setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      if (doublePress) sequencerRunning = !sequencerRunning;
      break;
      
    case 1:
      // sequencer mode 0, active sequence 1
      activeSeq = 1;
      if (sequencerRunning) updateSequence();
      setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      if (doublePress) sequencerRunning = !sequencerRunning;
      break;
      
    case 2:
      // sequencer mode 0, active sequence 2
      activeSeq = 2;
      if (sequencerRunning) updateSequence();
      setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      if (doublePress) sequencerRunning = !sequencerRunning;
      break;
      
    case 3:
      // sequencer mode 0, active sequence 3
      activeSeq = 3;
      if (sequencerRunning) updateSequence();
      setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      if (doublePress) sequencerRunning = !sequencerRunning;
      break;      
    
    case 7:
      // change sequencer time mode
      
      Music.setCutoff((1023-analogRead(pot1))*64);
      Music.setCutoffModAmount((1023-analogRead(pot2))*64);
    
      if (sequencerRunning) updateSequence();
      if (buttonPress1) {
        stepTime = analogRead(pot2)/2;
      }
      
      break;
  } 
}

void setCutoffFromPots() {
  Music.setCutoff((1023-analogRead(pot1))*64);
  Music.setCutoffModAmount((1023-analogRead(pot2))*64);
}

void updateLEDs() {
  for (int i = 0; i<8; i++) {
    if (seqNote[activeSeq*8+i] != -1) {
      SoftPWMSet(seqLed[i], 100);
    } else {
      SoftPWMSet(seqLed[seqStep], 0);
    }
  
  }
}

void resetPots() {
  
}

void noteInputFromBodyswitches() {
  for (int i = 0; i<8; i++) {
    if (bodySwitchVal[i] > 0) {
      
      //Serial.print(i);
      //Serial.print(": ");
      //Serial.print(bodySwitchVal[i]);
      
      int note = map(bodySwitchVal[i],0,127,-1,6);
      
      //Serial.print("\tnote: ");
      //Serial.print(note);
      
      if (note != -1) {
        seqNote[activeSeq*8+i] = scale[1][note];
      } else {
        seqNote[activeSeq*8+i] = -1;
      }
      
      //Serial.print("\tseqNote: ");
      //Serial.println(seqNote[i]);
    }
  }
}

void readPots() {
  int newPotVal1 = 1023-analogRead(pot1);
  int newPotVal2 = 1023-analogRead(pot2);
  
  if ( (newPotVal1 < (potVal1-potNoise)) || (newPotVal1 > (potVal1+potNoise)) ) {
    potVal1 = newPotVal1;
    potMoved1 = true;
  } else {
    potMoved1 = false;
  }
  if ( (newPotVal2 < (potVal2-potNoise)) || (newPotVal2 > (potVal2+potNoise)) ) {
    potVal2 = newPotVal2;
    potMoved2 = true;
  } else {
    potMoved2 = false;
  }
  

}

void changeMode() {
  
  // select mode with body switches
  
  int highest = -1, highestVal = averageNoise;
  for (int i=0; i<8; i++) {
    if (bodySwitchVal[i] > highestVal) {
      highest = i;
      highestVal = bodySwitchVal[i];
    }
  }
  if (highest != -1) {
    mode = highest;
  }
  
  /*
  // change from pot
  int newMode = map(1023-analogRead(pot1),0,1023,0,8);
  newMode = constrain(newMode,0,7);
  if (newMode != mode) { // only do something if mode has changed
    Serial.print("new mode ");
    Serial.println(newMode);
    mode = newMode;
    for (int i = 0; i<8; i++) {
      if (mode == i) {
        SoftPWMSet(seqLed[i],255);
      } else {
        SoftPWMSet(seqLed[i],0);
      }
    }
  }
  */
}

void changePreset() {
  int newPreset = map(1023-analogRead(pot2),0,1023,0,63);
  if (preset != newPreset) { // only do something if preset has changed
    preset = newPreset;
    Music.getPreset(preset);
  }
}



void updateSequence() {

  if (lastStep + stepTime < millis()) {
    //SoftPWMSet(seqLed[seqStep], 255);
    seqStep++;
    if (seqStep > seqEnd) seqStep = seqStart;
    
    int note = activeSeq*8+seqStep;  
    if (seqNote[note] != -1) {
      Music.noteOn(bassNote+seqNote[note]);
    }
    
    lastStep = millis();
  }
  
  // turn LED off just before step
  if (lastStep + stepTime - (stepTime/5) < millis()) {
    SoftPWMSet(seqLed[seqStep], 0);
  } else {
    SoftPWMSet(seqLed[seqStep], 255);
  }
}


void readBodyswitches() {
  for (int i = 0; i<8; i++) {
    
    int reading = constrain(analogRead (bodySwitch[i]),0,127);
    maxBodyReading = max(maxBodyReading, reading);
    
    if (reading > averageNoise) { // averageNoise is sampled on startup
      int midiVal = map (reading, averageNoise, maxBodyReading, 0, 127);
      usbMIDI.sendControlChange(i, constrain(midiVal,0,127), 1);
      bodySwitchVal[i] = constrain(midiVal,0,127);
    } else {
      usbMIDI.sendControlChange(i, 0, 1);
      bodySwitchVal[i] = 0;
      reading = 0;
    }
    seqLedVal[i] = reading*2;
  }
  
  usbMIDI.sendControlChange(8, averageNoise, 1);
  usbMIDI.sendControlChange(9, maxBodyReading, 1);
  
  maxBodyReading = maxBodyReading * maxBodyFadeout;
  
  // read potentiometers
  int reading = analogRead(pot1);
  usbMIDI.sendControlChange(10, (1023-reading)/8, 1);
  reading = analogRead(pot2);
  usbMIDI.sendControlChange(11, (1023-reading)/8, 1);
  
}

void printFeedback() {
  for (int i = 0; i<8; i++) {
    int reading = analogRead (bodySwitch[i]);

    seqLedVal[i] = reading/4;
    Serial.print(i);
    Serial.print(": ");
    Serial.print(reading);
    Serial.print("\t");
  }
  Serial.println();
  int potVal1 = analogRead(pot1);
  int potVal2 = analogRead(pot2);
  Serial.print(potVal1);
  Serial.print("\t");
  Serial.print(potVal2);
  Serial.println();
  

}

void startupAnimation() {
  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[i],HIGH);
    delay(50);
    digitalWrite(seqLed[i],LOW);  
  }
  
  pinMode(statusLed1, OUTPUT);
  pinMode(statusLed2, OUTPUT);
  digitalWrite(statusLed1, HIGH);
  digitalWrite(statusLed2, HIGH);
  delay(50);
  digitalWrite(statusLed1, LOW);
  digitalWrite(statusLed2, LOW);
}

void readButtons() {
  if (digitalRead(button1) == false && digitalRead(button2) == false) {
    if (doublePress == false) {
      doublePress = true;
      Serial.println("doublePress");
    }
  } else {
    doublePress = false;
  }
  
  if (digitalRead(button1) == HIGH) { // active low
    if (buttonPress1 == true) {
      // button has been released
      buttonRelease1 = true;
      Serial.println("button 1 released");
      buttonPress1 = false;
    }
  }
  if (digitalRead(button2) == HIGH) { // active low
    if (buttonPress2 == true) {
      // button has been released
      buttonRelease2 = true;
      Serial.println("button 2 released");
      buttonPress2 = false;
    }
  }
  
  if (digitalRead(button1) == false) {
    if (buttonPress1 == false) {
      Serial.println("button 1 pressed");
      buttonPress1 = true;
    } else {
      buttonRelease1 = false;
    }
  }
  
  if (digitalRead(button2) == false) {
    if (buttonPress2 == false) {
      Serial.println("button 2 pressed");
      buttonPress2 = true;
    } else {
      buttonRelease2 = false;
    }
  }
  
}

