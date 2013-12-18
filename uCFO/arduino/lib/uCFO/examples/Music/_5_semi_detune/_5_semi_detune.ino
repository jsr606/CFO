// You can set the number of oscillators (1 to 3) and the bit depth of the
// oscillators (8 or 12 bit). These settings must be defined before the
// inclusion of the MMM library files. They default to 1 osciallator
// and 8bit respectively.

#define NUM_OSCILLATORS 3
#define BIT_DEPTH 8

// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
#include <Music.h>

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.setFrequency(110);
  Music.setDetune(0.005); // ranges from 0.00 - 0.02 are usually good
  Music.setWaveform(DIGI2);
  Music.setSemitone1(0);
  Music.setSemitone2(7);
  Music.setSemitone3(-12);


}

void loop() {

}

