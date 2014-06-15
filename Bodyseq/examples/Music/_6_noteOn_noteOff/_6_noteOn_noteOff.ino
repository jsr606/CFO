#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

boolean noteIsOn = false;
int n = 0;
int dir = 1;
int rootNote = 36;
int note[] = {0,12,3,19,15,9,5,24,7};

long time = 0;
long lastTime = 0;
long timeDelay = 60;

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
  // Choosing the square wave oscillator.
  Music.setWaveform(SAW);
  
  // Detuning the three oscillators slightly to create movement in the sound.
  Music.setDetune(0.005);  
    
  // enabling the envelope lets us define an gain envelope for the synth
  // without having to specify it in our loop() or physics code.
  Music.enableEnvelope1();
  Music.setEnv1Attack(8);
  Music.setEnv1Decay(90);
  Music.setEnv1Sustain(48);
  Music.setEnv1Release(64);

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
