// You can set the number of oscillators (1 to 3) and the bit depth of the
// oscillators (8 or 12 bit). These settings must be defined before the
// inclusion of the MMM library files. They default to 1 osciallator
// and 8bit respectively.

#define NUM_OSCILLATORS 1
#define BIT_DEPTH 12

// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects wit "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
#include <Music.h>


void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();

}

void loop() {
      
}

