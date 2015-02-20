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

#define NUM_TRACKS 8
#define NUM_STEPS 8
#define NUM_LEDS 8

int mode;

int track[8];
int debounceTime = 40;

int _bpm;

const int scale[] = {0, 2, 3, 5, 7, 8, 10, 12};
//const int octave[] = {-24, -12, 0, 12, 24};
int rootNote = 36;

int trackPlaying = 0 ;
int trackSelected = 0;
int stepSelected = 0;
int noteSelected = 0;
int oct = 0;

int notes[64];
int octave[64];
int noteValues[8];

int leds;
int note;

//int var = 0;
//const int pot1 = A0, pot2 = A1;

// old stuff
const int seqLed[] = {3,4,5,6,7,8,9,10};
const int statusLed1 = 13;
//boolean debug = true;


/////////////
// BUTTONS //
/////////////
#define NUM_BUTTONS 8
const int buttonPin [] = {11,12,2};
int buttonIndex = 0;
int buttonRead = 0;
int buttonChange = 0;
int buttonState[] = {0, 0, 0};
unsigned long buttonNow = 0;
unsigned long buttonTime[] = {0, 0, 0};
int machineState = 0;

//////////
// KEYS //
//////////
#define NUM_KEYS 8
#define KEY_THRESHOLD 15
const int keyPin[] = {A2,A3,A4,A5,A6,A7,A8,A9};
int keyIndex = 0;
int keyRead = 0;
int keyValue = 0;
int keyChange = 0;
int keyState[] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long keyNow = 0;
unsigned long keyTime[] = {0, 0, 0, 0, 0, 0, 0, 0};
int keys;


void setup() {
  Music.init();
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  Music.getPreset(13);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
  analogReadAveraging(32);
  delay(2000);
  Sequencer.init(120);
  setupSequences();
  initInterface();  
}


void loop() {
  Sequencer.update();
  usbMIDI.read();
  Midi.checkSerialMidi();
  readButtons();
  readKeys();
  checkBPM();
  Music.setCutoffModAmount((analogRead(A1))*64);

  //  if(buttonChange || keyChange) {
    switch(machineState) {
      case 0:
        playTrack();
        break;
      case 1:
        selectNote();
        break;
      case 2:
        selectStep();
        break;
      case 3: // nothing
        break;
      case 4:
        selectTrack();
        break;
      case 5: // nothing
        break;
      case 6: // nothing
        break;
      case 7: // nothing
        break;
      default:
        break;
    }
    updateLEDs();
//    buttonChange = 0;
//  }
}
  
  
void playTrack() {
  if(keyChange) {
    Serial.println("PLAY TRACK");
  // code here
  keyChange = 0;
  }
}

void selectNote() {
  noteSelected = notes[stepSelected + 8 * trackSelected];
  oct = octave[stepSelected + 8 * trackSelected];
  if(keyChange) {
    for(int i = 0; i < NUM_KEYS-1; i++) {
      if(keys & (1 << i)) noteSelected = i;
      notes[stepSelected + 8 * trackSelected] = noteSelected;
    }
    if(keys & (1 << 7)) {
      oct ^= 1;
      octave[stepSelected + 8 * trackSelected] = oct;
    }
    for(int i = 0; i < NUM_STEPS; i++) {
      noteValues[i] = rootNote + scale[notes[8 * trackSelected + i]] + octave[i + 8 * trackSelected] * 12;
      Sequencer.insertNotes(track[trackSelected], noteValues, 8, 0);
    }        
    keyChange = 0;    
  }
}

void selectStep() {
  if(keyChange) {
    Serial.println("SELECT STEP");
    for(int i = 0; i < NUM_KEYS; i++) {
  // code here
      if(keys & (1 << i)) {
        stepSelected = i;
        Serial.print("Step selected: ");
        Serial.println(stepSelected);
      }
    }
    keyChange = 0;
  }
}

void selectTrack() {
//  if(keys > 0) {
  if(keyChange) {
    Serial.println("SELECT TRACK");
    for(int i = 0; i < NUM_KEYS; i++) {
//      Serial.print("Rechecking sequence on position ");
//      Serial.print(i);
//      Serial.print(". It is set to sequence index ");
//      Serial.println(track[i]);
      Sequencer.setInternal(track[i], false);
      if(keys & (1 << i)) {
        trackSelected = i;
        Serial.print("trackSelected shows ");
        Serial.print(trackSelected);
        Serial.print(" - Track selected: ");
        Serial.println(track[trackSelected]);
      }
      Sequencer.setInternal(track[trackSelected], true);
    }
    keyChange = 0;
  }
  // code here
}


void setupSequences() {
  for(int i = 0; i < NUM_TRACKS; i++) {
    for(int j = 0; j < NUM_STEPS; j++) {
      notes[8*i + j] = j;
    }
    track[i] = Sequencer.newSequence(NOTE_16, 8, LOOP);
    Serial.print("Track created for sequence ");
    Serial.print(track[i]);
    Serial.print(" on position ");
    Serial.println(i);
    Sequencer.startSequence(track[i]);
    for(int j = 0; j < NUM_STEPS; j++) {
      noteValues[j] = rootNote + scale[notes[8 * i + j]] + octave[8 * i + j] * 12;
    }
    Sequencer.insertNotes(track[i], noteValues, 8, 0);
    Sequencer.setInternal(track[i], true);
    Sequencer.setExternal(track[i], false);
    Serial.print("Internal set to ");
    Serial.println(Sequencer.getInternal(track[i]));
  }
  for(int i = 0; i < NUM_TRACKS; i++) {
    Serial.print("Rechecking sequence on position ");
    Serial.print(i);
    Serial.print(" - set to sequence index ");
    Serial.println(track[i]);
  }
}
