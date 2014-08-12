#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

const int seqLed [] = {3,4,5,6,7,8,9,10};
int seqLedVal [] = {0,0,0,0,0,0,0,0};
const int statusLed1 = 13;
const int button1 = 11, button2 = 12, button3 = 2;
const int pot1 = A0, pot2 = A1;
const int bodySwitch [] = {A2,A3,A4,A5,A6,A7,A8,A9};

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
int baseNote = 36;

const int scale[3][7] = {
  {0,2,4,5,7,9,11}, // major
  {0,2,3,5,6,7,10}, // blues
  {0,3,4,7,9,10,-1} // rock
};

const int scaleLength = 24;
int activeScale [scaleLength];

int bodySwitchVal [] = {0,0,0,0,0,0,0,0};

boolean bodySwitchesTouched = false;

int mode = 0;
int preset = 16;

boolean buttonPress1 = false, buttonPress2 = false, buttonPress3 = false, buttonRelease1 = false, buttonRelease2 = false, buttonRelease3 = false, doublePress = false;

int potVal1 = 0, potVal2 = 0;
boolean pot1Moved = false, pot2Moved = false, potsMoved = false;
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
  
  Serial.begin(9600);
  
  delay(100);

  Serial.println("hello");

  
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  
//  SoftPWMBegin();
  
  newActiveScale(scaleLength);
  
  startupAnimation();
  sampleAverageNoise();  
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
      if (pot2Moved) changePreset();
    }
    
    if (doublePress) {
      if (pot1Moved) {
        changeOctave();
        potsMoved = false;
      }
      int lowestSwitch = 0, highestSwitch = 0;
      if (bodySwitchesTouched) {
        for (int i = 0; i<8; i++) {
          if (bodySwitchVal[i] > 0) {
            highestSwitch = i;
          }
          if (bodySwitchVal[7-i] > 0) {
            lowestSwitch = 7-i;
          }
        }
        seqStart = lowestSwitch;
        seqEnd = highestSwitch;
      }
      
      
    }
    
    updateLEDs();
    lastInput = millis();
  
  }
  
  switch (mode) {
    case 0:
      // sequencer mode 0, active sequence 0
      activeSeq = 0;
      if (sequencerRunning) updateSequence();
      if (potsMoved) setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      break;
      
    case 1:
      // sequencer mode 0, active sequence 1
      activeSeq = 1;
      if (sequencerRunning) updateSequence();
      if (potsMoved) setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      break;
      
    case 2:
      // sequencer mode 0, active sequence 2
      activeSeq = 2;
      if (sequencerRunning) updateSequence();
      if (potsMoved) setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      break;
      
    case 3:
      // sequencer mode 0, active sequence 3
      activeSeq = 3;
      if (sequencerRunning) updateSequence();
      if (potsMoved) setCutoffFromPots();
      if (buttonPress1) noteInputFromBodyswitches();
      if (doublePress) sequencerRunning = !sequencerRunning;
      break;      
    
    case 7:
      // change sequencer time mode
      
//      Music.setCutoff((analogRead(pot1))*64);
//      Music.setCutoffModAmount((analogRead(pot2))*64);
      Music.setCutoffModAmount((analogRead(pot1))*64);
      Music.setResonance((analogRead(pot2))*64);
    
      if (sequencerRunning) updateSequence();
      if (buttonPress1) {
        stepTime = analogRead(pot2)/2;
      }
      
      break;
  } 
}

void sampleAverageNoise() {
  Serial.print("sampling average noise levels: ");
  
  for (int i = 0; i<8; i++) {
    pinMode(seqLed[i], OUTPUT);
//    SoftPWMSet(seqLed[i], 0);
//    SoftPWMSetFadeTime(seqLed[i], 0, 0);
    averageNoise += analogRead(bodySwitch[i]);
  }
  
  averageNoise = averageNoise/8 + 3;
  Serial.println(averageNoise);

}

void setCutoffFromPots() {
//  Music.setCutoff((analogRead(pot1))*64);
//  Music.setCutoffModAmount((analogRead(pot2))*64);
  Music.setCutoffModAmount((analogRead(pot1))*64);
  Music.setResonance((analogRead(pot2))*64);
}

void updateLEDs() {
  for (int i = 0; i<8; i++) {
    if (seqNote[activeSeq*8+i] != -1) {
//      SoftPWMSet(seqLed[i], 100);
      digitalWrite(seqLed[i], HIGH);
    } else {
//      SoftPWMSet(seqLed[seqStep], 0);
      digitalWrite(seqLed[seqStep], LOW);
    }
  
  }
}

void resetPots() {
  
}

void newActiveScale (int length) {
  Serial.println("lets make a new active scale");
  int amountOfNotesInScale = 0;
  for (int i = 0; i<7; i++) {
    if (scale[1][i] != -1) amountOfNotesInScale++;
  }
  
  for (int i = 0; i<length+1; i++) {
    int currentOctave = i/amountOfNotesInScale;
    Serial.print("octave ");
    Serial.print(currentOctave);
    int currentNote = scale[1][i%amountOfNotesInScale];
    Serial.print("\tnote ");
    Serial.print(currentNote);
    activeScale[i] = currentOctave*12+currentNote;
    Serial.print("\tthis yields ");
    Serial.println(activeScale[i]);
  }
}

void noteInputFromBodyswitches() {
  for (int i = 0; i<8; i++) {
    if (bodySwitchVal[i] > 0) {
      
      Serial.print(i);
      Serial.print(": ");
      Serial.print(bodySwitchVal[i]);
      
      int note = map(bodySwitchVal[i],0,127,-1,scaleLength-1);
      
      Serial.print("\tnote: ");
      Serial.print(note);
      
      if (note != -1) {
        seqNote[activeSeq*8+i] = activeScale[note];
      } else {
        seqNote[activeSeq*8+i] = -1;
      }
      
      Serial.print("\tseqNote: ");
      Serial.println(seqNote[activeSeq*8+i]);
    }
  }
}

void readPots() {
  int newPotVal1 = 1023-analogRead(pot1);
  int newPotVal2 = 1023-analogRead(pot2);
  
  potsMoved = false;
  
  if ( (newPotVal1 < (potVal1-potNoise)) || (newPotVal1 > (potVal1+potNoise)) ) {
    potVal1 = newPotVal1;
    pot1Moved = true;
    potsMoved = true;
  } else {
    pot1Moved = false;
  }
  if ( (newPotVal2 < (potVal2-potNoise)) || (newPotVal2 > (potVal2+potNoise)) ) {
    potVal2 = newPotVal2;
    pot2Moved = true;
    potsMoved = true;
  } else {
    pot2Moved = false;
  }
  

}

void changeOctave() {

  // change from pot
  int newOctave = map(analogRead(pot1),0,1023,-3,4);
  
  baseNote = 36 + newOctave*12;

  for (int i = 0; i<8; i++) {
    if (newOctave+3 == i) {
//      SoftPWMSet(seqLed[i],255);
      digitalWrite(seqLed[i],HIGH);
    } else {
//      SoftPWMSet(seqLed[i],0);
      digitalWrite(seqLed[i],LOW);
    }
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
}

void changePreset() {
  int newPreset = map(1023-analogRead(pot2),0,1023,0,32);
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
      Music.noteOn(baseNote+seqNote[note]);
    }
    
    lastStep = millis();
  }
  
  // turn LED off just before step
  if (lastStep + stepTime - (stepTime/5) < millis()) {
//    SoftPWMSet(seqLed[seqStep], 0);
    digitalWrite(seqLed[seqStep], LOW);
  } else {
//    SoftPWMSet(seqLed[seqStep], 255);
    digitalWrite(seqLed[seqStep], HIGH);
  }
}


void readBodyswitches() {
  bodySwitchesTouched = false;
  for (int i = 0; i<8; i++) {
    
    int reading = constrain(analogRead (bodySwitch[i]),0,127);
    maxBodyReading = max(maxBodyReading, reading);
    
    if (reading > averageNoise) { // averageNoise is sampled on startup
      int midiVal = map (reading, averageNoise, maxBodyReading, 0, 127);
      bodySwitchVal[i] = constrain(midiVal,0,127);
      bodySwitchesTouched = true;
    } else {
      bodySwitchVal[i] = 0;
      reading = 0;
    }
    seqLedVal[i] = reading*2;
  }
  
  maxBodyReading = maxBodyReading * maxBodyFadeout;
}

void printFeedback() {
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

void startupAnimation() {
  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[i],HIGH);
    delay(50);
    digitalWrite(seqLed[i],LOW);  
  }
  
  pinMode(statusLed1, OUTPUT);
  digitalWrite(statusLed1, HIGH);
  delay(50);
  digitalWrite(statusLed1, LOW);
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
  if (digitalRead(button3) == HIGH) { // active low
    if (buttonPress3 == true) {
      // button has been released
      buttonRelease3 = true;
      Serial.println("button 3 released");
      buttonPress3 = false;
    }
  }
  
  if (digitalRead(button1) == LOW) {
    if (buttonPress1 == false) {
      Serial.println("button 1 pressed");
      buttonPress1 = true;
    } else {
      buttonRelease1 = false;
    }
  }
  
  if (digitalRead(button2) == LOW) {
    if (buttonPress2 == false) {
      Serial.println("button 2 pressed");
      buttonPress2 = true;
    } else {
      buttonRelease2 = false;
    }
  }
  
  if (digitalRead(button3) == LOW) {
    if (buttonPress3 == false) {
      Serial.println("button 3 pressed");
      buttonPress3 = true;
    } else {
      buttonRelease3 = false;
    }
  }
  
}

