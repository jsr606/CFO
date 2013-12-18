// You can set the number of oscillators (1 to 3) and the bit depth of the
// oscillators (8 or 12 bit). These settings must be defined before the
// inclusion of the MMM library files. They default to 1 osciallator
// and 8bit respectively.

#define NUM_OSCILLATORS 1
#define BIT_DEPTH 8

// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
#include <Music.h>

int delayTime = 2000;
int cnt = 0;
long timeNow;
long lastTime = 0;

byte waveFormArray[] = { SINE,
                        SQUARE,
                        PULSE,
                        TRIANGLE,
                        SAW,
                        FUZZ,
                        DIGI1,
                        DIGI2,
                        DIGI3,
                        DIGI4,
                        NOISE,
                        DIGI6,
                        TAN1,
                        TAN2,
                        TAN3,
                        TAN4
                       };

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.setFrequency(220);
  Music.setWaveform(SINE);

}

void loop() {
  
  timeNow = millis();
  if((timeNow-lastTime) > delayTime) {
    cnt = cnt + 1;
    if(cnt>15) cnt = 0;
    Music.setWaveform(waveFormArray[cnt]);
    lastTime = timeNow;
  }   
}

