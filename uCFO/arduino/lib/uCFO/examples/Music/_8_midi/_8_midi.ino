// You can set the number of oscillators (1 to 3) and the bit depth of the
// oscillators (8 or 12 bit). These settings must be defined before the
// inclusion of the MMM library files. They default to 1 osciallator
// and 8bit respectively.

#define NUM_OSCILLATORS 3
#define BIT_DEPTH 8
#define MIDI

// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
#define NUM_OSCILLATORS 3
#define BIT_DEPTH 8
#define MIDI
#define MIDI_CHANNEL 1
#include <Music.h>

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.enableEnvelope();
  Midi.init();

}

void loop() {
  
  // In order to send MIDI to the sketch, use the Music_Controls.pde Processing sketch
  Midi.checkMidi();
  
}

