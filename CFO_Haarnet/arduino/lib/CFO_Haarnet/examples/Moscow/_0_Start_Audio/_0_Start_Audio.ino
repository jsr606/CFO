#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
//  Music.enableEnvelope1();
//  Music.enableEnvelope2();

// These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  
// Loading a preset from EEPROM
  Music.getPreset(16);

// Uncomment below if you want another midi note  
//  Music.noteOn(48,127);

}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  
}
