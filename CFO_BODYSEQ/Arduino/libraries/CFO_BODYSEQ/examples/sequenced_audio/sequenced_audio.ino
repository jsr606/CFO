#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

// sequence ID
int s1;
// sequence step index
int indx1 = 0;
const int nbr_notes1 = 16;
const int notes1[] = {12, 24, 7, 12, 36, 12, 24, 15, 0, 12, 48, 36, 19, 24, 3, 36};

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  Music.getPreset(13);
  
// These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
    
    // this is the sequencer code
  Sequencer.init(140);
//  Midi.setMidiIn(false);
//  Midi.setMidiThru(false);
//  Midi.setMidiOut(false);
//  Midi.setMidiClockIn(false);
//  Midi.setMidiClockThru(false);
//  Midi.setMidiClockOut(true);
  Sequencer.setInternalClock(true);  

  s1 = Sequencer.newSequence(NOTE_16, &s1cb);
  Sequencer.setInternal(s1, true);
  Sequencer.setExternal(s1, false);
  Sequencer.startSequence(s1);
  
// Loading a preset from EEPROM
}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  Sequencer.update();
  Midi.checkSerialMidi();
}

// callback function for the step sequencer

void s1cb() {
  Music.noteOn(notes1[indx1++] + 24, 127);
  if(indx1 >= nbr_notes1) indx1 = 0;
}
