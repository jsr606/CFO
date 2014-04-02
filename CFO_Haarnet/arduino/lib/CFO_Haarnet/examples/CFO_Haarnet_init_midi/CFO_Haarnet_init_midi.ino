// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
//#define MIDI
#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  Music.setWaveform1(SAW);
  Music.setEnv1Sustain(127);
//  Music.setEnv2Sustain(127);
  Music.setCutoff(45*256);
  Music.setGain2(0.0);
  Music.setGain3(0.0);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);  
}

void loop() {

  usbMIDI.read();
}
