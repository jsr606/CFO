#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

// sequence ID
int s1, s2;
// sequence step index
int indx1 = 0;
int indx2 = 0;
int indx3 = 0;
const int nbr_notes1 = 16;
const int nbr_notes2 = 8;
const int nbr_notes3 = 32;
const int notes1[] = {12, 24, 19, 12, 36, 12, 24, 15, 36, 12, 48, 36, 19, 24, 15, 36};

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  
// Loading a preset from EEPROM
  Music.getPreset(13);
  
// These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
    
  // this is the sequencer code
  Sequencer.init(128);
  Sequencer.setInternalClock(true);  

  //Sequencer.newSequence(CALLBACK, SUBDIV);
  // create new sequence and ID (s1)
  s1 = Sequencer.newSequence(NOTE_8, &s1cb);
  s2 = Sequencer.newSequence(NOTE_6, &s2cb);

  // start sequence 1
  Sequencer.startSequence(s1);
  Sequencer.startSequence(s2);
  
  Music.setPortamento(124);

}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  Sequencer.update();
  Midi.checkSerialMidi();  
}

// callback function for the step sequencer

void s1cb() {
  Music.noteOn(notes1[indx1++] + 36, 127);
  if(indx2 >= nbr_notes1) indx1 = 0;
  indx3++;
  if(indx3 >= nbr_notes3) {
    indx1 = 0;
    indx2 = 0;
    indx3 = 0;
  }
}

void s2cb() {
  Music.noteOn(notes1[indx1++] + 36, 127);
  if(indx1 >= nbr_notes2) indx2 = 0;
}
