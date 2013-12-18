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

boolean noteIsOn = false;
int n = 0;
int dir = 1;
int rootNote = 36;
int note[] = {0,2,3,5,7,9,10,12,14};

long time = 0;
long lastTime = 0;
long timeDelay = 80;

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
  // Choosing the square wave oscillator.
  Music.setWaveform(DIGI3);
  
  // Detuning the three oscillators slightly to create movement in the sound.
  Music.setDetune(0.008);  
    
  // enabling the envelope lets us define an gain envelope for the synth
  // without having to specify it in our loop() or physics code.
  Music.enableEnvelope();
  Music.setAttack(8);
  Music.setDecay(90);
  Music.setSustain(48);
  Music.setRelease(64);

}

void loop() {
  
  // This short routine loops note over and over again
  time = millis();
  if(time - lastTime > timeDelay) {
    if(!noteIsOn) {
      Music.noteOn(rootNote+note[n]);
      noteIsOn = true;
      n = n + dir;
      if(n > 7)
      {
        dir = -1;
      }
      else if(n < 1)
      {
        dir = 1;
      }
    } else {
      Music.noteOff();
      noteIsOn = false;
    }
    lastTime = time;
  }

}

