#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <uCFO.h>

// sequence ID
int s1, s2;
// sequence step index
int indx1 = 0;
int indx2 = 0;
int indx3 = 0;
const int nbr_notes1 = 16;
const int nbr_notes2 = 8;
const int nbr_notes3 = 32;
const int notes1[] = {12, 24, 7, 12, 36, 12, 24, 15, 0, 12, 48, 36, 19, 24, 3, 36};

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
  Sequencer.init(128);

  //Sequencer.newSequence(CALLBACK, SUBDIV);
  // create new sequence and ID (s1)
  s1 = Sequencer.newSequence(&s1cb, NOTE_8);
  s2 = Sequencer.newSequence(&s2cb, NOTE_6);

  // start sequence 1
  Sequencer.startSequence(s1);
  Sequencer.startSequence(s2);
  
// Loading a preset from EEPROM
  Music.getPreset(20);
  Music.setPortamento(124);
}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  Sequencer.update();
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
  Music.noteOn(notes1[indx1++] + 48, 127);
  if(indx1 >= nbr_notes2) indx2 = 0;
}
