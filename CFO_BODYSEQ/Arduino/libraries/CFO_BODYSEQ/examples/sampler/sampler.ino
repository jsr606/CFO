#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

// sequence ID
int s1, s2, s3, s4;

// sequence step index
int indx1 = 0;
int indx2 = 0;
int indx3 = 0;
int indx4 = 0;

const int nbr_notes1 = 16;
const int nbr_steps2 = 8;
const int nbr_notes3 = 8;

const int notes1[] = {12, 24, 7, 12, 36, 12, 24, 15, 0, 12, 48, 36, 19, 24, 3, 36};
const int midiCC[] =     {8,  8, 11, 21, 21, 11, 21, 31, 8};
const int midiValue[] =  {0, 80, 68, 68, 68, 72, 72, 72, 0};

int BD[] = {1, 0, 0, 1, 0, 0, 1, 0};
int SN[] = {0, 0, 0, 0, 1, 0, 0, 0};
int RS[] = {0, 1, 0, 1, 0, 0, 1, 0};
int CP[] = {0, 0, 0, 0, 1, 0, 0, 0};
int HH[] = {1, 0, 1, 1, 0, 1, 0, 0};
int OH[] = {0, 0, 0, 0, 0, 0, 0, 0};
int CL[] = {0, 0, 1, 0, 0, 1, 0, 0};
int CB[] = {0, 0, 0, 0, 0, 0, 0, 1};

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();

  Music.enableEnvelope1();
  Music.enableEnvelope2();

// Loading a preset from EEPROM
  Music.getPreset(21);
    
// These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
  
// this is the sequencer code
  Sequencer.init(120);
//  Midi.setMidiIn(false);
//  Midi.setMidiThru(false);
//  Midi.setMidiOut(false);
//  Midi.setMidiClockIn(false);
//  Midi.setMidiClockThru(false);
//  Midi.setMidiClockOut(true);
  Sequencer.setInternalClock(true);  

  //Sequencer.newSequence(SUBDIV, CALLBACK);
  // create new sequences and IDs
  s1 = Sequencer.newSequence(NOTE_16, &s1cb);
  s2 = Sequencer.newSequence(NOTE_32, &s2cb);
  s3 = Sequencer.newSequence(NOTE_8, &s3cb);
  s4 = Sequencer.newSequence(NOTE_16, &s4cb);
  Sequencer.setInternal(s1, true);
  Sequencer.setExternal(s1, false);
  Sequencer.setInternal(s2, true);
  Sequencer.setExternal(s2, false);
  Sequencer.setInternal(s3, true);
  Sequencer.setExternal(s3, false);
  Sequencer.setInternal(s4, true);
  Sequencer.setExternal(s4, false);

  // start sequences
  Sequencer.startSequence(s1);
  Sequencer.startSequence(s2);
  Sequencer.startSequence(s3); // start s4 instead of s3
  
}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  Sequencer.update();
  Midi.checkSerialMidi();
}

// callback function for the step sequencers

void s1cb() {
  Music.noteOn(notes1[indx1++] + 24, 127);
  if(indx1 >= nbr_notes1) indx1 = 0;
}

void s2cb() {
  Midi.controller(MIDI_CHANNEL - 1, midiCC[indx2], midiValue[indx2]);
  indx2++;
  if(indx2 >= nbr_steps2) indx2 = 0;
}

void s3cb() {
  if(BD[indx3]) Music.noteOnSample(0);
  if(SN[indx3]) Music.noteOnSample(1);
  if(RS[indx3]) Music.noteOnSample(2);
  if(CP[indx3]) Music.noteOnSample(3);
  if(HH[indx3]) Music.noteOnSample(4);
  if(OH[indx3]) Music.noteOnSample(5);
  if(CL[indx3]) Music.noteOnSample(6);
  if(CB[indx3]) Music.noteOnSample(7);

  indx3++;
  if(indx3 >= nbr_notes3) indx3 = 0;
}

void s4cb() {
  if(BD[indx3]) Music.noteOnSample(0);
  if(SN[indx3]) Music.noteOnSample(1);
  if(RS[indx3]) Music.noteOnSample(2);
  if(CP[indx1]) Music.noteOnSample(3);
  if(HH[indx4++]) Music.noteOnSample(4);
  if(OH[indx3]) Music.noteOnSample(5);
  if(CL[indx4]) Music.noteOnSample(6);
  if(CB[indx3++]) Music.noteOnSample(7);

  indx3++;
  if(indx3 >= nbr_notes3) indx3 = 0;
  if(indx2 >= nbr_notes1) indx4 = 0;
}
