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
#define NUM_SAMPLES 8
#define NUM_LEDS 8

int mode;
unsigned long debounceTime = 100;
unsigned long debounceTimeDefault = 100;
unsigned long debounceTimeLong = 500;

int _bpm;

int trackPlaying = 0 ;
int trackSelected = 0;
int sampleSelected = 0;
int stepSelected = 0;
int trackChained = -1;

int s1;
int indx = 0;
int indxLED;
uint8_t sample[NUM_TRACKS][NUM_SAMPLES][NUM_STEPS];

int leds;
int chainedLedState = 0;
unsigned long ledNow;
unsigned long ledTime = 0;
unsigned long ledPulse = 100;

bool inStartupMode = true;


// old stuff
const int seqLed[] = {3,4,5,6,7,8,9,10};
const int statusLed1 = 13;
//boolean debug = true;

/////////////
// BUTTONS //
/////////////
#define NUM_BUTTONS 3
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
  Music.setSampler(true);
  Music.setSynth(false);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
  analogReadAveraging(32);
  Sequencer.init(120);
//  Sequencer.setInternalClock(true);  
  s1 = Sequencer.newSequence(NOTE_16, &s1cb);
//  resetMemory();
  loadSequences();
  initInterface();
  Sequencer.startSequence(s1);

}


void loop() {

  Sequencer.update();
  usbMIDI.read();
  Midi.checkSerialMidi();
  readButtons();
  readKeys();
  checkBPM();
  checkBitcrush();

  switch(machineState) {
    case 0:
      playTrack();
      break;
    case 1:
      selectStep();
      break;
    case 2:
      selectSample();
      break;
    case 3:
      selectTrack();
      break;
    case 4:
      selectTrack();
      break;
    case 5: // nothing
      chainTrack();
      break;
    case 6:
      copyTrack();
      break;
    case 7:
      clearTrack();
      break;
    default:
      break;
  }
  updateLEDs();
}
  
  
void playTrack() {
  if(keyChange && keys) {
    Serial.println("PLAY TRACK");
    for(int i = 0; i < NUM_KEYS; i++) {
      if(keys & (1 << i)) {
        if(trackPlaying == i) trackChained = -1;
        if(trackChained < 0) trackPlaying = i;
        else trackChained = i;
      }
    }
  keyChange = 0;
  }
}

void selectStep() {
  if(keyChange) {
    Serial.println("SELECT STEP");
    for(int k = 0; k < NUM_KEYS; k++) {
      if(keys & (1 << k)) {
        stepSelected = k;
        int j = sampleSelected;
        int i = trackSelected;
        sample[i][j][k] ^= 1;
        EEPROM.write(k + NUM_SAMPLES * (j + i * NUM_TRACKS), sample[i][j][k]);
      }
    }
    keyChange = 0;
  }
}


void selectSample() {
  if(keyChange) {
    Serial.println("SELECT SAMPLE");
    for(int i = 0; i < NUM_KEYS; i++) {
      if(keys & (1 << i)) {
        sampleSelected = i;
      }
    }
    keyChange = 0;
  }
}

void selectTrack() {
  if(keyChange) {
    Serial.println("SELECT TRACK");
    for(int i = 0; i < NUM_KEYS; i++) {
      if(keys & (1 << i)) {
        trackSelected = i;
      }
    }
    keyChange = 0;
  }
}

void chainTrack() {
  if(keyChange) {
    Serial.println("SELECT TRACK");
    for(int i = 0; i < NUM_KEYS; i++) {
      if(keys & (1 << i)) {
        if(i == trackChained) trackChained = -1;
        else trackChained = i;
      }
    }
    keyChange = 0;
  }  
}

void copyTrack() {
  if(keyChange) {
    debounceTime = debounceTimeLong;
    Serial.println("COPY TRACK");
    for(int i = 0; i < NUM_KEYS; i++) {
      if(keys & (1 << i)) {
        for(int j=0; j<NUM_SAMPLES; j++) {
          for(int k=0; k<NUM_STEPS; k++) {
            sample[i][j][k] = sample[trackSelected][j][k];
            EEPROM.write(k + NUM_SAMPLES * (j + i * NUM_TRACKS), sample[i][j][k]);
          }
        }
      }
    }
    keyChange = 0;
    debounceTime = debounceTimeDefault;
  }
}

void clearTrack() {
  if(keyChange) {
    Serial.println("CLEAR TRACK");
    for(int i = 0; i < NUM_KEYS; i++) {
      if(keys & (1 << i)) {
        for(int j=0; j<NUM_SAMPLES; j++) {
          for(int k=0; k<NUM_STEPS; k++) {
            sample[i][j][k] = 0;
            EEPROM.write(k + NUM_SAMPLES * (j + i * NUM_TRACKS), 0);
          }
        }
      }
    }
    keyChange = 0;
  }
}


void resetMemory() {
  for(int i=0; i<NUM_TRACKS; i++) {
    for(int j=0; j<NUM_SAMPLES; j++) {
      for(int k=0; k<NUM_STEPS; k++) {
        EEPROM.write(k + NUM_SAMPLES * (j + i * NUM_TRACKS), 0);
      }
    }
  }  
}

void loadSequences() {
  for(int i=0; i<NUM_TRACKS; i++) {
    for(int j=0; j<NUM_SAMPLES; j++) {
      for(int k=0; k<NUM_STEPS; k++) {
        sample[i][j][k] = EEPROM.read(k + NUM_SAMPLES * (j + i * NUM_TRACKS));
      }
    }
  }  
}

void s1cb() {
  indxLED = indx;
  for(int i=0; i<NUM_SAMPLES; i++) {
    if(sample[trackPlaying][i][indx]) Music.noteOnSample(i);
  }
  indx++;
  if(indx >= NUM_STEPS) {
    indx = 0;
    if(trackChained < 0);
    else {
      int t = trackPlaying;
      trackPlaying = trackChained;
      trackChained = t;
    }
  }
}

