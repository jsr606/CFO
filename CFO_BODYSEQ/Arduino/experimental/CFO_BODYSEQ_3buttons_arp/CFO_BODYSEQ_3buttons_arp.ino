//______________________/\\\\\__________________________/\\\______________________________/\\\__________________________________________________________        
// ____________________/\\\///__________________________\/\\\_____________________________\/\\\__________________________________________________________       
//  ___________________/\\\______________________________\/\\\_____________________________\/\\\_____/\\\__/\\\_______________________________/\\\\\\\\___      
//   _____/\\\\\\\\__/\\\\\\\\\_______/\\\\\______________\/\\\____________/\\\\\___________\/\\\____\//\\\/\\\___/\\\\\\\\\\_____/\\\\\\\\___/\\\////\\\__     
//    ___/\\\//////__\////\\\//______/\\\///\\\____________\/\\\\\\\\\____/\\\///\\\____/\\\\\\\\\_____\//\\\\\___\/\\\//////____/\\\/////\\\_\//\\\\\\\\\__    
//     __/\\\____________\/\\\_______/\\\__\//\\\___________\/\\\////\\\__/\\\__\//\\\__/\\\////\\\______\//\\\____\/\\\\\\\\\\__/\\\\\\\\\\\___\///////\\\__   
//      _\//\\\___________\/\\\______\//\\\__/\\\____________\/\\\__\/\\\_\//\\\__/\\\__\/\\\__\/\\\___/\\_/\\\_____\////////\\\_\//\\///////__________\/\\\__  
//       __\///\\\\\\\\____\/\\\_______\///\\\\\/_____________\/\\\\\\\\\___\///\\\\\/___\//\\\\\\\/\\_\//\\\\/_______/\\\\\\\\\\__\//\\\\\\\\\\________\/\\\\_ 
//        ____\////////_____\///__________\/////_______________\/////////______\/////______\///////\//___\////________\//////////____\//////////_________\////__
//         CFO BODYSEQ sequencer, 3 button version, reworked interaction, http://www.vsionhairies.info/

#define MIDI_CHANNEL 1
#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

const int seqLed [] = {3,4,5,6,7,8,9,10};
int seqLedVal [] = {0,0,0,0,0,0,0,0};

const int statusLed1 = 13;
const int buttonPin [] = {11,12,2}; 

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

boolean buttonState [] = {HIGH, HIGH, HIGH};
boolean buttonAction [] = {false, false, false};

int potVal1 = 0, potVal2 = 0;
boolean pot1Moved = false, pot2Moved = false, potsMoved = false;
int potNoise = 1;

boolean debug = true;

int currentArpNote = 0;
int arpSpeed = 300;
unsigned long lastArp = millis();
int arpNotes = 0;
int arpNote[8] = {-1,-1,-1,-1,-1,-1,-1,-1};    

boolean justButtonInteraction = true;

int lastKey = -1;

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
  pinMode(buttonPin[2], INPUT_PULLUP);

  newActiveScale(scaleLength);

  pinMode(statusLed1,OUTPUT);
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

    // general navigation
    // button 0 pushed
    if (buttonState[0] == LOW) {
      noteInputFromBodyswitches();
    }
    
    // button 1 pushed
    if (buttonState[1] == LOW) {
      eraseNotesFromBodyswitches();
      if (pot1Moved) {
        // change sequencer speed
        stepTime = map(analogRead(pot1),0,1023,1000,0);
        potsMoved = false; 
      }
    }  
    
    // button 2 pushed
    if (buttonState[2] == LOW) {
      if (buttonAction[2] == true) justButtonInteraction = true;
      
      changeMode();
      if (activeSeq != mode) justButtonInteraction = false;
      activeSeq = mode;
      
      if (pot2Moved) {
        changePreset();
        potsMoved = false;
        justButtonInteraction = false;
      }
      if (pot1Moved) {
        changeOctave();
        potsMoved = false;
        justButtonInteraction = false;
      }
    }
    
    //button 2 har been released, turn sequencer ON/OFF 
    if (buttonState[2] == HIGH) {
      if (buttonAction[2] == true && justButtonInteraction == true) {
        sequencerRunning = !sequencerRunning;
        Serial.println("sequencer toggle");
        if (sequencerRunning == true) {
          digitalWrite(statusLed1, HIGH);
          Serial.println("HIGH");
        } else {
          digitalWrite(statusLed1, LOW);
          Serial.println("LOW");
        } 
      }
    }

    
    // button 0 and 2 pushed
    if (buttonState[2] == LOW && buttonState[0] == LOW) {
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
    lastInput = millis();
  }
  
  if (bodySwitchesTouched && buttonState[0] == HIGH && buttonState[1] == HIGH && buttonState[2] == HIGH) {
    
    
    //Serial.print("arp notes");
    arpNotes = 0;
    
    for (int i = 0; i<8; i++) {
      if (bodySwitchVal[i] > 0) {

        digitalWrite(seqLed[i], HIGH);
        int oct = map(bodySwitchVal[i],0,127,-2,3);
        
        //Serial.print("oct");
        
        arpNote[arpNotes] = baseNote+activeScale[i]+oct*12;
        
        //Serial.print("\t");
        //Serial.print(arpNote[arpNotes]);
  
        arpNotes++;
        //sequencerRunning = false;
        
      } else {
        digitalWrite(seqLed[i], LOW);
      }
    }
    //Serial.println();

    if (arpNotes == 1) {
      Serial.println("just one note");
      if (arpNote[0] != lastNote) {
        Music.noteOn(arpNote[0]);
        lastNote = arpNote[0];
      }
    }
    
    if (arpNotes > 1) {

      if (lastArp + arpSpeed < millis() ) {
        Serial.print("doing the arp ");
        
        Music.noteOn(arpNote[currentArpNote]);
        Serial.println(arpNote[currentArpNote]);
        lastArp = millis();
        currentArpNote++;      
        
        if (currentArpNote>arpNotes-1) currentArpNote = 0;
        
        arpSpeed = map(analogRead(pot1),0,1023,1000,0);
        potsMoved = false;
        
      }
    }

    
  }
  
  if (sequencerRunning) updateSequence();
  if (potsMoved) setCutoffFromPots();
}

void sampleAverageNoise() {
  Serial.print("sampling average noise levels: ");

  for (int i = 0; i<8; i++) {
    pinMode(seqLed[i], OUTPUT);
    averageNoise += analogRead(bodySwitch[i]);
  }
  averageNoise = averageNoise/8 + 3;
  Serial.println(averageNoise);
}

void setCutoffFromPots() {
  Music.setCutoff((analogRead(pot1))*64);
  Music.setCutoffModAmount((analogRead(pot2))*64);
}

void updateLEDs() {
  for (int i = 0; i<8; i++) {
    if (seqNote[activeSeq*8+i] != -1) {
      digitalWrite(seqLed[i], HIGH);
    } 
    else {
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
  int newPotVal1 = 1023-analogRead(pot1);
  int newPotVal2 = 1023-analogRead(pot2);


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
  int newOctave = map(analogRead(pot1),0,1023,-3,4);

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
  int newPreset = map(analogRead(pot2),0,1023,63,0);
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
      int midiVal = map (reading, averageNoise+5, maxBodyReading, 0, 127);
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
  
  delay(100);
}

void readButtons() {
  // buttons are active low
  
  justButtonInteraction = true;
  
  for (int i = 0; i<3; i++) {
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


