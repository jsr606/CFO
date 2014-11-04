#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <uCFO.h>

// sequence ID
int s1, s2, s3;
// sequence step index
int indx1 = 0;
int indx2 = 0;
int indx3 = 0;
const int nbr_steps1 = 16;
const int nbr_steps2 = 8;
const int nbr_steps3 = 15;
const int nbr_steps4 = 4;
const int notes1[] = {12, 24, 7, 15, 36, 12, 24, 15, 0, 12, 48, 36, 19, 24, 3, 36};
const int notes2[] = {0, 7, 15, 24, 12, 19, 3, 24};
const int midiCC2[] =     {8,  8, 11, 21, 31, 11, 21, 31, 8};
const int midiValue2[] =  {0, 80, 68, 68, 68, 72, 72, 72, 0};
const int midiCC3[] =     {14, 24,  34, 14, 34, 24,  14, 34, 24, 14,  34, 14, 24, 14,  24, 34};
const int midiValue3[] =  {1, 2, 4, 2, 1, 4, 2, 1, 4, 2, 1, 4, 2, 2, 1, 4};
const int midiCC4[] =     {11, 11, 11, 11};
const int midiValue4[] =  {64, 67, 71, 76};

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  
// These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  
//  delay(5000);
  
    // this is the sequencer code
  Sequencer.init(112);

  //Sequencer.newSequence(CALLBACK, SUBDIV);
  // create new sequence and ID (s1)
  s1 = Sequencer.newSequence(&s1cb, NOTE_8);
  s2 = Sequencer.newSequence(&s2cb, NOTE_32);
  s3 = Sequencer.newSequence(&s3cb, NOTE_96);

  // start sequence 1
  Sequencer.startSequence(s1);
//  Sequencer.startSequence(s2);
  Sequencer.startSequence(s3);
  
// Loading a preset from EEPROM
  Music.getPreset(19);
  Music.setGain2(0.0);
  Music.setGain3(0.0);
}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  Sequencer.update();
}

// callback function for the step sequencer

void s1cb() {
  Music.noteOn(notes1[indx1++] + 48, 127);
  if(indx1 >= nbr_steps1) indx1 = 0;
}

void s2cb() {
  if(indx2 < nbr_steps2) Midi.controller(MIDI_CHANNEL - 1, midiCC2[indx2], midiValue2[indx2]);
  indx2++;
  if(indx2 >= nbr_steps2) indx2 = 0;
}

void s3cb() {
  if(indx3 < nbr_steps4) Midi.controller(MIDI_CHANNEL - 1, midiCC4[indx3], midiValue4[indx3]);
  indx3++;
  if(indx3 >= nbr_steps4) indx3 = 0;
}
