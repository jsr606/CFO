//______________________/\\\\\__________________________/\\\______________________________/\\\__________________________________________________________        
// ____________________/\\\///__________________________\/\\\_____________________________\/\\\__________________________________________________________       
//  ___________________/\\\______________________________\/\\\_____________________________\/\\\_____/\\\__/\\\_______________________________/\\\\\\\\___      
//   _____/\\\\\\\\__/\\\\\\\\\_______/\\\\\______________\/\\\____________/\\\\\___________\/\\\____\//\\\/\\\___/\\\\\\\\\\_____/\\\\\\\\___/\\\////\\\__     
//    ___/\\\//////__\////\\\//______/\\\///\\\____________\/\\\\\\\\\____/\\\///\\\____/\\\\\\\\\_____\//\\\\\___\/\\\//////____/\\\/////\\\_\//\\\\\\\\\__    
//     __/\\\____________\/\\\_______/\\\__\//\\\___________\/\\\////\\\__/\\\__\//\\\__/\\\////\\\______\//\\\____\/\\\\\\\\\\__/\\\\\\\\\\\___\///////\\\__   
//      _\//\\\___________\/\\\______\//\\\__/\\\____________\/\\\__\/\\\_\//\\\__/\\\__\/\\\__\/\\\___/\\_/\\\_____\////////\\\_\//\\///////__________\/\\\__  
//       __\///\\\\\\\\____\/\\\_______\///\\\\\/_____________\/\\\\\\\\\___\///\\\\\/___\//\\\\\\\/\\_\//\\\\/_______/\\\\\\\\\\__\//\\\\\\\\\\________\/\\\\_ 
//        ____\////////_____\///__________\/////_______________\/////////______\/////______\///////\//___\////________\//////////____\//////////_________\////__
//         CFO BODYSEQ sequencer, 2 button version, reworked interaction, http://www.vsionhairies.info/

#define MIDI_CHANNEL 1
#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

const int seqLed [] = {2,3,4,5,6,7,8,9};
int seqLedVal [] = {0,0,0,0,0,0,0,0};

const int statusLed1 = 10, statusLed2 = 11;
const int buttonPin [] = {12,13}; 

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
int seqNote[] = {-1,-1,0,-1,12,-1,-1,-1,-1,-1,0,-1,24,-1,-1,-1,-1,-1,0,-1,36,-1,-1,-1,-1,-1,0,-1,0,-1,-1,-1,-1,-1,0,-1,12,-1,-1,-1,-1,-1,0,-1,24,-1,-1,-1,-1,-1,0,-1,36,-1,-1,-1,-1,-1,0,-1,0,-1,-1,-1};
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

boolean buttonState [] = {HIGH, HIGH};
boolean buttonAction [] = {false, false};

int potVal1 = 0, potVal2 = 0;
boolean pot1Moved = false, pot2Moved = false, potsMoved = false;
int potNoise = 1;

boolean debug = true;

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

  pinMode(buttonPin[0], INPUT_PULLUP);
  pinMode(buttonPin[1], INPUT_PULLUP);

  newActiveScale(scaleLength);

  pinMode(statusLed1,OUTPUT);
  pinMode(statusLed2,OUTPUT);
  for (int i = 0; i<8; i++) {
    pinMode(seqLed[i], OUTPUT);
  }
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

    for (int i = 0;i<2;i++) {
      if (buttonAction[i] = true) {
        if (buttonState[i] == LOW) {
          if (i==0) digitalWrite(statusLed1,HIGH);
          if (i==1) digitalWrite(statusLed2,HIGH);
          usbMIDI.sendControlChange(i,1,1);
        } else {
          usbMIDI.sendControlChange(i,0,1);
          if (i==0) digitalWrite(statusLed1,LOW);
          if (i==1) digitalWrite(statusLed2,LOW);
        }
      }
    }
    
    
    for (int i = 0; i<8; i++) {
      usbMIDI.sendControlChange(10+i,bodySwitchVal[i],1);
      if (bodySwitchVal[i]>0) {
        digitalWrite(seqLed[i],HIGH);
      } else {
        digitalWrite(seqLed[i],LOW);
      }
    }
  
    
    if (potsMoved) {
      int midiPot1 = map(potVal1,1023,0,0,127);
      int midiPot2 = map(potVal2,1023,0,0,127);
      usbMIDI.sendControlChange(20,midiPot1,1);
      usbMIDI.sendControlChange(21,midiPot2,1);
    }
    
    
    lastInput = millis();
  }

}

void sampleAverageNoise() {
  Serial.print("sampling average noise levels: ");

  for (int i = 0; i<8; i++) {
    averageNoise += analogRead(bodySwitch[i]);
  }
  averageNoise = averageNoise/8 + 3;
  Serial.println(averageNoise);
}

void setCutoffFromPots() {
  Music.setCutoff((1023-analogRead(pot1))*64);
  Music.setCutoffModAmount((1023-analogRead(pot2))*64);
}

void updateLEDs() {
  for (int i = 0; i<8; i++) {
    if (seqNote[activeSeq*8+i] != -1) {
      digitalWrite(seqLed[i], HIGH);
    } else {
      digitalWrite(seqLed[i], LOW);
    }
  }
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

void eraseNotesFromBodyswitches() {
  for (int i = 0; i<8; i++) {
    if (bodySwitchVal[i] > 0) {
      seqNote[activeSeq*8+i] = -1;
    }
  }
  updateLEDs();
}

void noteInputFromBodyswitches() {
  for (int i = 0; i<8; i++) {
    if (bodySwitchVal[i] > 0) {

      if (debug) Serial.print(i);
      if (debug) Serial.print(": ");
      if (debug) Serial.print(bodySwitchVal[i]);

      int note = map(bodySwitchVal[i],0,127,0,scaleLength-1);

      if (debug) Serial.print("\tnote: ");
      if (debug) Serial.print(note);

      seqNote[activeSeq*8+i] = activeScale[note];
      
      if (debug) Serial.print("\tseqNote: ");
      if (debug) Serial.println(seqNote[activeSeq*8+i]);
    }
  }
  updateLEDs();
}

void readPots() {
  int newPotVal1 = analogRead(pot1);
  int newPotVal2 = analogRead(pot2);


  potsMoved = false;

  if ( (newPotVal1 < (potVal1-potNoise)) || (newPotVal1 > (potVal1+potNoise)) ) {
    potVal1 = newPotVal1;
    pot1Moved = true;
    potsMoved = true;
  } 
  else {
    pot1Moved = false;
  }
  if ( (newPotVal2 < (potVal2-potNoise)) || (newPotVal2 > (potVal2+potNoise)) ) {
    potVal2 = newPotVal2;
    pot2Moved = true;
    potsMoved = true;
  } 
  else {
    pot2Moved = false;
  }

  if (potsMoved) {
    Serial.print("pot 1 ");
    Serial.println(potVal1);
    Serial.print("pot 2 ");
    Serial.println(potVal2);
  }

}

void changeOctave() {

  // change from pot
  int newOctave = map(analogRead(pot1),1023,0,-3,4);

  baseNote = 36 + newOctave*12;

  for (int i = 0; i<8; i++) {
    if (newOctave+3 == i) {
      //      SoftPWMSet(seqLed[i],255);
      digitalWrite(seqLed[i],HIGH);
    } 
    else {
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
  updateLEDs();
}

void changePreset() {
  int newPreset = map(analogRead(pot2),1023,0,63,0);
  if (preset != newPreset) { // only do something if preset has changed
    // NB! user preset 0-16 might be empty, resulting in crazy sounds!
    if (debug) Serial.print("new preset ");
    if (debug) Serial.println(newPreset);
    preset = newPreset;
    Music.getPreset(preset);
  }
}

void updateSequence() {

  if (lastStep + stepTime < millis()) {

    seqStep++;
    if (seqStep > seqEnd) seqStep = seqStart;

    int note = activeSeq*8+seqStep;  

    if (seqNote[note] != -1) {
      Music.noteOn(baseNote+seqNote[note]);
    }
    lastStep = millis();

  }

  updateLEDs();

  int blinkTime = max(stepTime/5,100);

  if (lastStep + blinkTime < millis()) {
    digitalWrite(seqLed[seqStep], HIGH);
  }
  if (lastStep + stepTime - blinkTime < millis()) {
    digitalWrite(seqLed[seqStep], LOW);
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
    } 
    else {
      bodySwitchVal[i] = 0;
      reading = 0;
    }
    seqLedVal[i] = reading*2;
  }

  maxBodyReading = maxBodyReading * maxBodyFadeout;  
}

void printFeedback() {
  // for debugging purposes
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

  digitalWrite(statusLed1, HIGH);
  digitalWrite(statusLed2, HIGH);

  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[i],HIGH);
    delay(30); 
  }
  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[i],LOW);
    delay(30); 
  }


  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[7-i],HIGH);
    delay(30); 
  }
  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[7-i],LOW);
    delay(30); 
  }

  digitalWrite(statusLed1, LOW);
  digitalWrite(statusLed2, LOW);
  
  delay(100);
}

void readButtons() {

  // buttons are active low
  for (int i = 0; i<2; i++) {
    if (digitalRead(buttonPin[i]) == HIGH) {
      if (buttonState[i] == LOW) {
        // button has just been released
        buttonAction[i] = true;
        if (debug) Serial.print("button ");
        if (debug) Serial.print(i);
        if (debug) Serial.println(" has just been released");
      } else {
        // button was already released, no action here       
        buttonAction[i] = false;
      }
      buttonState[i] = HIGH;
    }

    if (digitalRead(buttonPin[i]) == LOW) {
      if (buttonState[i] == HIGH) {
        // button has just been pushed
        buttonAction[i] = true;
        if (debug) Serial.print("button ");
        if (debug) Serial.print(i);
        if (debug) Serial.println(" has just been pushed");     
        buttonState[i] = LOW;
      } else {
        // button was already psuhed, no action here
        buttonAction[i] = false;
      }
    } 
  }
}
