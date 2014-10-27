#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>

// sequence ID
int s1;
// sequence step index
int indx1 = 0;
const int nbr_notes = 16;
const int notes[] = {12, 24, 7, 12, 36, 12, 24, 15, 0, 12, 48, 36, 19, 24, 3, 36};

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
  Music.enableEnvelope1();
  Music.enableEnvelope2();

// These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  
    // this is the sequencer code
  Sequencer.init();

  //Sequencer.newSequence(BPM, CALLBACK, SUBDIV);
  // create new sequence and ID (s1)
  s1 = Sequencer.newSequence(128, &s1cb, 16);      

  // start sequence 1
  Sequencer.startSequence(s1);
  
// Loading a preset from EEPROM
  Music.getPreset(16);

// Uncomment below if you want another midi note  
//  Music.noteOn(48,127);

}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  Sequencer.update();
}

// callback function for the step sequencer

void s1cb() {
  Music.noteOn(notes[indx1++] + 24, 127);
  if(indx1 >= nbr_notes) indx1 = 0;
}
