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

int delayTime = 400;
int cnt = 0;
float baseFrequency = 110;
long timeNow;
long lastTime = 0;

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.setFrequency(220);
  Music.setDetune(0.001); // ranges from 0.00 - 0.02 are usually good
  Music.setWaveform(SAW);

}

void loop() {
  
  timeNow = millis();
  if((timeNow-lastTime) > delayTime) {
    cnt = cnt + 1;
    if(cnt>3) cnt = 0;
    Music.setFrequency1(baseFrequency*1*cnt);
    Music.setFrequency2(baseFrequency*1.3333*cnt);
    Music.setFrequency3(baseFrequency*1.5*cnt);
    lastTime = timeNow;
  }   
}

