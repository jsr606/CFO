#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_PROTOSEQ.h>

// sequence ID
int s1;

//int notes1[] = {12, 24,  7, 12, 36, 12, 24, 15, 0, 12, 48, 36, 19, 24, 3, 36};
int notes1[] = {36, 48, 31, 36, 60, 36, 48, 39, 24, 36, 72, 60, 43, 48, 27, 60};

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
  
  analogReadAveraging(32);
  
  Sequencer.init(140);
  
  s1 = Sequencer.newSequence(NOTE_16, 16, LOOP);
  Sequencer.startSequence(s1);
  Sequencer.insertNotes(s1, notes1, 16, 0);
  Sequencer.setInternal(s1, true);
  Sequencer.setExternal(s1, false);

  Midi.setMidiIn(false);
  Midi.setMidiThru(false);
  Midi.setMidiOut(false);
  Midi.setMidiClockIn(false);
  Midi.setMidiClockThru(false);
  Midi.setMidiClockOut(false);
  Sequencer.setInternalClock(true);
  
}

void loop() {
  Sequencer.update();
  usbMIDI.read();
  Midi.checkSerialMidi();
}

