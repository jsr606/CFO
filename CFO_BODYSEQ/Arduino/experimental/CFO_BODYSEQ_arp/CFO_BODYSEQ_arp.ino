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
#include "CFO_BODYSEQ.h"

const int seqLed [] = {3, 4, 5, 6, 7, 8, 9, 10};
int seqLedVal [] = {0, 0, 0, 0, 0, 0, 0, 0};

int _bpm = 120;
int s1;

const int statusLed1 = 13;
const int buttonPin [] = {11, 12, 2};

const int pot1 = A0, pot2 = A1;
const int bodySwitch [] = {A2, A3, A4, A5, A6, A7, A8, A9};

unsigned long lastPrint = millis();

boolean sequencerRunning = true;

float maxBodyReading = 0;
float maxBodyFadeout = 0.99;

float averageNoise = 0, noiseThreshold = 25;

int inputFreq = 10;
unsigned long lastInput = millis();

int seqStep = 0;
int seqLength = 7;

int seqNote[] = {
  24, 18,  0, 12,  0, 18, 17,  2,
  0, -1,  0,  7, 24,  0, -1, -1,
  0, -1,  0, 24,  2,  0, 39,  0,
  -1,  2,  0, -1,  0,  3,  0,  6,
  6,  2,  0, -1, -1,  0, 15, 17,
  3, -1,  0, -1,  0, 15, 39, 15,
  -1, -1,  0, -1, 36,  2,  3,  0,
  -1,  0,  0, -1,  0,  7, 18,  2
};

int activeSeq = 0, seqStart = 0, seqEnd = 7;
unsigned long lastStep = millis();
int stepTime = 200, blinkTime = 50;
int baseNote = 36;

const int scale[3][7] = {
  {0, 2, 4, 5, 7, 9, 11}, // major
  {0, 2, 3, 5, 6, 7, 10}, // blues
  {0, 3, 4, 7, 9, 10, -1} // rock
};

const int scaleLength = 24;
int activeScale [scaleLength];

int bodySwitchVal [] = {0, 0, 0, 0, 0, 0, 0, 0};

boolean bodySwitchesTouched = false;

int mode = 1;
int bank = 0, preset = 16;

boolean buttonState [] = {HIGH, HIGH, HIGH};
boolean buttonAction [] = {false, false, false};
boolean buttonsPressed = false, onlyButtonsPressed = false;

int potVal1 = 0, potVal2 = 0;
boolean pot1Moved = false, pot2Moved = false, potsMoved = false;
int potNoise = 1;
unsigned long seqRest = millis();
unsigned long inputRest = millis();

int currentArpNote = 0;
unsigned long lastArp = millis();
int arpNotes = 0, lastArpNote = -1;
int arpNote[8] = { -1, -1, -1, -1, -1, -1, -1, -1};
boolean arpNoteOn[8] = {false, false, false, false, false, false, false, false};
int arpDivider = 4;
boolean arpActive = false;
boolean freezeArp = false;
int arpTranspose = 0;

boolean debug = false;

boolean randomizer = false;
int randFreqMin = 500, randFreqMax = 20000;
unsigned long nextRandom = random(randFreqMin, randFreqMax) + millis();

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
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);

  analogReadAveraging(32);

  delay(1000);

  Sequencer.init(120);
  s1 = Sequencer.newSequence(NOTE_16, &sequenceCallback);
  Sequencer.setInternalClock(true);
  Sequencer.setInternal(s1, true);
  Sequencer.setExternal(s1, true);
  Sequencer.startSequence(s1);
  checkBPM();

  Serial.begin(9600);

  Serial.println("hello");

  pinMode(buttonPin[0], INPUT_PULLUP);
  pinMode(buttonPin[1], INPUT_PULLUP);
  pinMode(buttonPin[2], INPUT_PULLUP);

  newActiveScale(scaleLength);

  pinMode(statusLed1, OUTPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(seqLed[i], OUTPUT);
  }

  startupAnimation();

  // sequencer is ON
  digitalWrite(statusLed1, HIGH);

  sampleAverageNoise();

  // check for button press on startup
  readButtons();
  if (buttonAction[0] == true) {
    randomizer = true;
    Serial.println("randomizer is on");
  }
}

void loop() {
  // check for incoming USB MIDI messages
  usbMIDI.read();
  Midi.checkSerialMidi();

  // update sequencer
  Sequencer.update(); // sequencer calls function sequenceCallback whenever a new step is reached

  // check user input at certain frequency (inputFreq)
  if (lastInput + inputFreq < millis()) {

    // check user input
    readButtons();
    readPots();
    readBodyswitches();

    // only update sequencer LEDs when arp is not active and user is not interacting with pots (seqRest)
    if (!arpActive) {
      if (seqRest < millis()) {
        updateLEDs();
      }
    }

    if (potsMoved && !buttonsPressed) setCutoffFromPots();

    if (buttonState[2] == LOW) {
      if (pot2Moved) {
        changePreset();
      }
      if (pot1Moved) {
        changeOctave();
      }
    }

    if (buttonAction[2] && buttonState[2] == HIGH) {
      Serial.println("input rest");
      inputRest = millis() + 300;
    }

    if (!buttonsPressed && bodySwitchesTouched) {
      arpActive = true;
    }

    if (arpActive) {
      // arp mode
      arp();
    } else {
      // sequencer mode
      sequencer();
    }
  }

  // randomizer mode is on if button 1 is pushed during boot
  if (randomizer) {
    if (millis() > nextRandom) {
      activeSeq = random(8);
      Serial.print("new random bank ");
      Serial.println(activeSeq);
      nextRandom = random(randFreqMin, randFreqMax) + millis();
    }
  }
}

void sequencer() {
  // standard sequence editor

  // button 0 released
  if (buttonAction[0] == true && buttonState[0] == HIGH && onlyButtonsPressed) {
    // start / stop sequencer
    sequencerRunning = !sequencerRunning;
    if (sequencerRunning) {
      Sequencer.start();
      digitalWrite(statusLed1, HIGH);
    } else {
      Sequencer.stop();
      Serial.println("stop");
      seqNoteDump();
      digitalWrite(statusLed1, LOW);
    }
  }

  // button 0 pushed
  if (buttonState[0] == LOW) {
    noteInputFromBodyswitches();
  }
  // button 1 pushed
  if (buttonState[1] == LOW) {
    eraseNotesFromBodyswitches();
    if (pot1Moved) {
      // change sequencer speed
      checkBPM();
    }
  }
  // button 2 pushed
  if (buttonState[2] == LOW) {
    changeSequenceBank();
    activeSeq = bank;

  }
  // button 0 and 2 pushed
  if (buttonState[2] == LOW && buttonState[0] == LOW) {
    int lowestSwitch = 0, highestSwitch = 0;
    if (bodySwitchesTouched) {
      for (int i = 0; i < 8; i++) {
        if (bodySwitchVal[i] > 0) {
          highestSwitch = i;
        }
        if (bodySwitchVal[7 - i] > 0) {
          lowestSwitch = 7 - i;
        }
      }
      seqStart = lowestSwitch;
      seqEnd = highestSwitch;
    }
  }
  lastInput = millis();


}

int highestSwitch() {
  int highestSwitch = -1, highestVal = -1;

  if (bodySwitchesTouched) {
    for (int i = 0; i < 8; i++) {
      if (bodySwitchVal[i] > highestVal) {
        highestVal = bodySwitchVal[i];
        highestSwitch = i;
      }
    }
  }
  return highestSwitch;
}

void seqNoteDump() {
  Serial.println("sequencer notes");
  int k = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (seqNote[k] < 10 && seqNote[k] > -1) {
        Serial.print(" ");
      }
      Serial.print(seqNote[k]);
      Serial.print(", ");
      k++;
    }
    Serial.println();
  }
}



void arp () {

  // do the arp yo
  if (!freezeArp) {
    arpNotes = 0;

    for (int i = 0; i < 8; i++) {
      if (bodySwitchVal[i] > 0) {

        digitalWrite(seqLed[i], HIGH);
        int oct = map(bodySwitchVal[i], 0, 127, -1, 2);

        arpNote[arpNotes] = activeScale[i] + oct * 12;
        arpNotes++;
        arpNoteOn[i] = true;

      } else {
        digitalWrite(seqLed[i], LOW);
        arpNoteOn[i] = false;
      }
    }
  }

  if (freezeArp) {
    // transpose arp while holding button
    if (buttonState[1] == LOW) {
      int s = highestSwitch();

      if (s > -1) {
        arpTranspose = s * 12 - 4 * 12;
      } else {
        arpTranspose = 0;
      }

      // exit arp by pushing button 1 and 2
      if (buttonAction[2] == true && buttonState[2] == LOW) {
        freezeArp = false;
      }
    }

    // update leds
    if (seqRest < millis()) {
      for (int i = 0; i < 8; i++) {
        if (arpNoteOn[i]) {
          digitalWrite(seqLed[i], HIGH);
        } else {
          digitalWrite(seqLed[i], LOW);
        }
      }
    }

  }

  if (arpNotes == 1) {
    if (arpNote[0] != lastArpNote) {
      if (lastArp + stepTime < millis()) {
        Music.noteOn(baseNote + arpNote[0] + arpTranspose);
        lastArpNote = arpNote[0];
        lastArp = millis();
      }
    }
  }

  if (arpNotes > 1) {

    if (lastArp + stepTime / arpDivider < millis() ) {

      Music.noteOn(baseNote + arpNote[currentArpNote] + arpTranspose);
      //Serial.println(arpNote[currentArpNote]);
      lastArp = millis();
      currentArpNote++;

      if (currentArpNote > arpNotes - 1) currentArpNote = 0;

      if (!buttonsPressed && pot2Moved && inputRest < millis()) arpDivider = map(analogRead(pot2), 0, 1023, 1, 6);

    }
  }

  if (arpNotes == 0) {
    lastArpNote = -1;
    arpActive = false;
  } else {
    if (buttonAction[0]) {
      if (buttonState[0] == LOW) {
        freezeArp = !freezeArp;
        if (freezeArp) Serial.println("freeze arp");
        if (!freezeArp) Serial.println("unfreeze arp");
      }
    }
  }
}

void setCutoffFromPots() {
  Music.setCutoff((analogRead(pot1)) * 64);
  Music.setCutoffModAmount((analogRead(pot2)) * 64);
}

void updateLEDs() {
  for (int i = 0; i < 8; i++) {
    if (seqNote[activeSeq * 8 + i] != -1) {
      // there is a note
      digitalWrite(seqLed[i], HIGH);
    }
    else {
      digitalWrite(seqLed[i], LOW);
    }
  }

  // blink ON to show sequencer progression
  if (lastStep + blinkTime < millis()) {
    digitalWrite(seqLed[seqStep], HIGH);
  }

  // blink OFF to show sequencer progression
  if (lastStep + stepTime - blinkTime / 2 < millis()) {
    digitalWrite(seqLed[seqStep], LOW);
  }
}

void newActiveScale (int length) {
  if (debug) Serial.println("lets make a new active scale");
  int amountOfNotesInScale = 0;
  for (int i = 0; i < 7; i++) {
    if (scale[1][i] != -1) amountOfNotesInScale++;
  }

  for (int i = 0; i < length + 1; i++) {
    int currentOctave = i / amountOfNotesInScale;
    if (debug) Serial.print("octave ");
    if (debug) Serial.print(currentOctave);
    int currentNote = scale[1][i % amountOfNotesInScale];
    if (debug) Serial.print("\tnote ");
    if (debug) Serial.print(currentNote);
    activeScale[i] = currentOctave * 12 + currentNote;
    if (debug) Serial.print("\tthis yields ");
    if (debug) Serial.println(activeScale[i]);
  }
}

void eraseNotesFromBodyswitches() {
  for (int i = 0; i < 8; i++) {
    if (bodySwitchVal[i] > 0) {
      seqNote[activeSeq * 8 + i] = -1;
    }
  }
}

void noteInputFromBodyswitches() {
  for (int i = 0; i < 8; i++) {
    if (bodySwitchVal[i] > 0) {

      if (debug) Serial.print(i);
      if (debug) Serial.print(": ");
      if (debug) Serial.print(bodySwitchVal[i]);

      int note = map(bodySwitchVal[i], 0, 127, 0, scaleLength - 1);

      if (debug) Serial.print("\tnote: ");
      if (debug) Serial.print(note);

      seqNote[activeSeq * 8 + i] = activeScale[note];

      if (debug) Serial.print("\tseqNote: ");
      if (debug) Serial.println(seqNote[activeSeq * 8 + i]);
    }
  }
}

void readPots() {
  int newPotVal1 = 1023 - analogRead(pot1);
  // Serial.println(analogRead(pot1));
  int newPotVal2 = 1023 - analogRead(pot2);
  // Serial.println(analogRead(pot2));
  potsMoved = false;

  if ( (newPotVal1 < (potVal1 - potNoise)) || (newPotVal1 > (potVal1 + potNoise)) ) {
    potVal1 = newPotVal1;
    pot1Moved = true;
    potsMoved = true;
    onlyButtonsPressed = false;
  }
  else {
    pot1Moved = false;
  }
  if ( (newPotVal2 < (potVal2 - potNoise)) || (newPotVal2 > (potVal2 + potNoise)) ) {
    potVal2 = newPotVal2;
    pot2Moved = true;
    potsMoved = true;
    onlyButtonsPressed = false;
  }
  else {
    pot2Moved = false;
  }
}

void changeOctave() {
  // change from pot 1
  int newOctave = map(analogRead(pot1), 0, 1023, -3, 4);
  baseNote = 36 + newOctave * 12;

  // display on LEDs which octave is active
  seqRest = millis() + 300;
  for (int i = 0; i < 8; i++) {
    if (newOctave + 3 == i) {
      digitalWrite(seqLed[i], HIGH);
    }
    else {
      digitalWrite(seqLed[i], LOW);
    }
  }
}

void changeSequenceBank() {

  // select bank with body switches

  int highest = -1, highestVal = averageNoise;
  for (int i = 0; i < 8; i++) {
    if (bodySwitchVal[i] > highestVal) {
      highest = i;
      highestVal = bodySwitchVal[i];
    }
  }
  if (highest != -1) {
    bank = highest;
    if (debug) Serial.print("playing sequence bank: ");
    if (debug) Serial.println(bank);
  }
  //updateLEDs();
}

void changePreset() {
  int newPreset = map(analogRead(pot2), 0, 1023, 0, 31);
  if (preset != newPreset) { // only do something if preset has changed
    // NB! user preset 0-16 might be empty, resulting in crazy sounds!
    if (debug) Serial.print("new preset ");
    if (debug) Serial.println(newPreset);
    preset = newPreset;
    Music.getPreset(preset);
  }

  // display on LEDs which preset is active
  for (int i = 0; i < 8; i++) {
    if (preset % 8 == i) {
      digitalWrite(seqLed[i], HIGH);
    }
    else {
      digitalWrite(seqLed[i], LOW);
    }
  }
  seqRest = millis() + 300;

}

void sequenceCallback() {
  // this is a callback function, called every time the sequencer steps
  // calculate how many millis have passed since last step

  unsigned long thisStep = millis();
  stepTime = thisStep - lastStep;
  lastStep = thisStep;

  // and calculate sequencer blink time
  blinkTime = max(stepTime >> 2, 100);

  // updater sequencer notes
  seqStep++;
  if (seqStep > seqEnd) seqStep = seqStart;

  int note = activeSeq * 8 + seqStep;

  if (seqNote[note] != -1 && arpActive == false) {
    Music.noteOn(baseNote + seqNote[note]);
  }
}

void updateSequence() {
  if (lastStep + stepTime < millis()) {

    seqStep++;
    if (seqStep > seqEnd) seqStep = seqStart;

    int note = activeSeq * 8 + seqStep;

    if (seqNote[note] != -1) {
      Music.noteOn(baseNote + seqNote[note]);
    }
    lastStep = millis();
  }
}


void readBodyswitches() {
  bodySwitchesTouched = false;
  for (int i = 0; i < 8; i++) {

    int reading = analogRead (bodySwitch[i]);
    maxBodyReading = max(maxBodyReading, reading);

    if (reading > averageNoise * 2) { // averageNoise is sampled on startup
      int midiVal = map (reading, averageNoise, maxBodyReading, 0, 127);
      bodySwitchVal[i] = constrain(midiVal, 0, 127);
      bodySwitchesTouched = true;
      onlyButtonsPressed = false;
    }
    else {
      bodySwitchVal[i] = 0;
      reading = 0;
    }
    seqLedVal[i] = reading * 2;
  }

  maxBodyReading = maxBodyReading * maxBodyFadeout;
}

void printFeedback() {
  for (int i = 0; i < 8; i++) {
    int reading = analogRead (bodySwitch[i]);
    seqLedVal[i] = reading / 4;
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
  // flashy leds

  digitalWrite(statusLed1, HIGH);

  for (int i = 0; i < 8; i++) {
    digitalWrite(seqLed[i], HIGH);
    delay(30);
  }
  for (int i = 0; i < 8; i++) {
    digitalWrite(seqLed[i], LOW);
    delay(30);
  }

  for (int i = 0; i < 8; i++) {
    digitalWrite(seqLed[7 - i], HIGH);
    delay(30);
  }
  for (int i = 0; i < 8; i++) {
    digitalWrite(seqLed[7 - i], LOW);
    delay(30);
  }

  digitalWrite(statusLed1, LOW);
  delay(100);
}

void readButtons() {
  // buttons are active low
  buttonsPressed = false;

  for (int i = 0; i < 3; i++) {
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
      buttonsPressed = true;
      if (buttonState[i] == HIGH) {
        // button has just been pushed
        buttonAction[i] = true;
        if (debug) Serial.print("button ");
        if (debug) Serial.print(i);
        if (debug) Serial.println(" has just been pushed");
        buttonState[i] = LOW;
        onlyButtonsPressed = true;
      } else {
        // button was already psuhed, no action here
        buttonAction[i] = false;
      }
    }
  }
}

void sampleAverageNoise() {
  Serial.println("sampling average noise levels: ");
  int samples = 20;
  for (int i = 0; i < 8; i++) {
    int reading = 0;
    for (int j = 0; j < samples; j++) {
      reading += analogRead(bodySwitch[i]);
      Serial.print(reading);
      Serial.print("\t");
    }
    reading = reading / samples;

    Serial.print("noise level on bodyswitch[");
    Serial.print(i);
    Serial.print("] is ");
    Serial.println(reading);
    averageNoise += reading;
  }
  averageNoise = averageNoise / 8 + noiseThreshold;
  Serial.println(averageNoise);
}

void checkBPM() {
  // >> is C for bitwise shift right.
  if (debug) Serial.println("checkBPM");
  int bpm = analogRead(A0) >> 2; // fast way of roughly dividing by 4
  if (bpm != _bpm) {
    _bpm = bpm;
    if (debug) Serial.print("BPM set to ");
    if (debug) Serial.println(_bpm);
    Sequencer.setbpm(_bpm);
    if (_bpm == 0) {
      Midi.setMidiIn(false);
      Midi.setMidiThru(false);
      Midi.setMidiOut(false);
      Midi.setMidiClockIn(true);
      Midi.setMidiClockThru(true);
      Midi.setMidiClockOut(false);
      Sequencer.setInternalClock(false);
    } else {
      Midi.setMidiIn(false);
      Midi.setMidiThru(false);
      Midi.setMidiOut(false);
      Midi.setMidiClockIn(false);
      Midi.setMidiClockThru(false);
      Midi.setMidiClockOut(true);
      Sequencer.setInternalClock(true);
      // Sequencer.sequencerContinue();
    }
  }
}
