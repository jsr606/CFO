#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include "CFO_BODYSEQ.h"

// sequence ID
int s1, s2;
int _bpm = 120;

// sequence step index
int indx1 = 0;
int indx2 = 0;
const int nbr_notes1 = 8;  // try with 16 :)
const int nbr_steps2 = 8;
int notes1[] = {12, 24,  7, 12, 36, 12, 24, 15, 0, 12, 48, 36, 19, 24, 3, 36};
//int notes1[] = {36, 48, 31, 36, 60, 36, 48, 39, 24, 36, 72, 60, 43, 48, 27, 60};
int notes2[] = {0, 2, 3, 5, 6, 8, 10, 12};
int vels[] = {100, 72, 96, 64, 112, 88, 78, 96};

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
  
  Sequencer.init(_bpm);
  
  s1 = Sequencer.newSequence(NOTE_16, 16, LOOP);
  Sequencer.startSequence(s1);
  Sequencer.insertNotes(s1, notes1, 16, 0);
  Sequencer.insertNotes(s1, vels, 7, 8); 
  Sequencer.insertNotes(s1, vels, 3, 2); 
  Sequencer.setInternal(s1, true);
  Sequencer.setExternal(s1, true  );
}

void loop() {
// Checking for incoming MIDI to use dashboard
  Sequencer.update();
  usbMIDI.read();
  Midi.checkSerialMidi();
  checkBPM();
}

void checkBPM() {
//  int bpm = analogRead(A0)>>2;
  int bpm = 124;
  if(bpm != _bpm) {
    _bpm = bpm;
    Serial.println(_bpm);
    Sequencer.setbpm(_bpm);
    if(_bpm == 0) {
      Midi.setMidiIn(false);
      Midi.setMidiThru(true);
      Midi.setMidiOut(false);
      Midi.setMidiClockIn(true);
      Midi.setMidiClockThru(true);
      Midi.setMidiClockOut(false);
      Sequencer.setInternalClock(false);
    } else {
      Midi.setMidiIn(false);
      Midi.setMidiThru(false);
      Midi.setMidiOut(true);
      Midi.setMidiClockIn(false);
      Midi.setMidiClockThru(false);
      Midi.setMidiClockOut(true);
      Sequencer.setInternalClock(true);
//      Sequencer.sequencerContinue();
    }
  }
}
