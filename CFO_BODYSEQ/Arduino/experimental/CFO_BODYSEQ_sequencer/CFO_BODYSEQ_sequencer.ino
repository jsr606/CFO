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

boolean sequencerRunning = false;

float maxBodyReading = 0;
float maxBodyFadeout = 0.9999;

float averageNoise = 0;

int inputFreq = 10;
unsigned long lastInput = millis();

int seqStep = 0;
int seqLength = 7;
int seqNote[] = {-1,-1,0,-1,12,-1,-1,-1};
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

  usbMIDI.read();

  if (lastInput + inputFreq < millis()) {
    
    readBodyswitches();
    
    checkButtons();
    if (doublePress) {
      changeMode();
      changePreset();
    }
    
    lastInput = millis();
  }
  
  /*
  if (lastPrint + 100 < millis()) {
    printFeedback();
    lastPrint = millis();
  }
  */

  
  

  
  switch (mode) {
    case 0:
      updateSequence();
      Music.setCutoff((1023-analogRead(pot1))*64);
      Music.setCutoffModAmount((1023-analogRead(pot2))*64);
      
      if (buttonPress1 && doublePress == false) {
        for (int i = 0; i<8; i++) {
          if (bodySwitchVal[i] > 0) {
            Serial.print(i);
            Serial.print(": ");
            Serial.print(bodySwitchVal[i]);
            int note = map(bodySwitchVal[i],0,127,-1,6);
            Serial.print("\tnote: ");
            Serial.print(note);
            if (note != -1) {
              seqNote[i] = scale[1][note];
            } else {
              seqNote[i] = -1;
            }
            Serial.print("\tseqNote: ");
            Serial.println(seqNote[i]);
          }
        }
      }
      
      break;
    
    case 8:
      stepTime = analogRead(pot2)/5+50;
      break;
  } 
}

void changeMode() {
  int newMode = map(1023-analogRead(pot1),0,1023,0,7);
  if (newMode != mode) { // only do something if mode has changed
    mode = newMode;
    for (int i = 0; i<8; i++) {
      if (mode == i) {
        SoftPWMSet(seqLed[i],255);
      } else {
        SoftPWMSet(seqLed[i],0);
      }
    }
  }
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
    SoftPWMSet(seqLed[seqStep],0);
    seqStep++;
    if (seqStep > seqLength) seqStep = 0;
    SoftPWMSet(seqLed[seqStep],255);
    
    if (seqNote[seqStep] != -1) {
      Music.noteOn(bassNote+seqNote[seqStep]);
    }
    lastStep = millis();
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

void checkButtons() {
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

