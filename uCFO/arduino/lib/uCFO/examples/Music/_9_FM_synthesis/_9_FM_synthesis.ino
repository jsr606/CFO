// You can set the number of oscillators (1 to 3) and the bit depth of the
// oscillators (8 or 12 bit). These settings must be defined before the
// inclusion of the MMM library files. They default to 1 oscallator
// and 8bit respectively.
#define FM  // tell the library to use FM synthesis
#define NUM_OSCILLATORS 2
#define BIT_DEPTH 12

#include <Music.h>

long time;
long lastTime = 0;
long eventTime = 10;
int cnt = 0;
int dir = 1;

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.setFM2(0);  // this value is already defaulted to 0 in Music.init()
  Music.setFrequency2(148.3);  // This is the base frequency of the audible oscillator 
  Music.setFrequency1(487.78);  // This is the modulating frequency

}

void loop() {
  // This code increases the FM amount from 0 to 127 and back again in intervals defined by "eventTime"
  time = millis();
  if(time-lastTime > eventTime) {
    Music.setFM2(cnt);
    cnt += dir;
    if(cnt <= 0 || cnt >= 127) dir *= -1;
    lastTime = time;
  }
  

}

