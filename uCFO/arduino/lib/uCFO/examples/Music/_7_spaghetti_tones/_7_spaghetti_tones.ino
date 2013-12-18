#define NUM_OSCILLATORS 3
#define BIT_DEPTH 12

// The Music and Midi objects are automatically instantiated when the header file is included.
// Make calls to the Music and Midi objects with "Music.function(args)" and "Midi.function(args)"
// You still need to call Music.init() and Midi.init() in the setup() function below.
#include <Music.h>

// variables for this sketch
float gain = 1.0;
float c = 220; // center frequency
float f1 = 1;
float f2 = 1;
float f3 = 1;
float m1 = 1.0011;
float m2 = 1.0012;
float m3 = 1.0013;


void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  
  // Choosing the sine wave oscillator (optional since this is already the default).
  Music.setWaveform(0);
  
  // Setting the initial frequency for all three oscillators.
  Music.setFrequency(c);
  
  // Detuning the three oscillators slightly to create movement in the sound.
  Music.setDetune(0.002);  
  
}

void loop() {
  
  // This short routine creates a

    Music.setFrequency1(c*f1);
    Music.setFrequency2(c*f2);
    Music.setFrequency3(c*f3);
    
    f1 *= m1;
    f2 *= m2;
    f3 *= m3;
    
    if(f1 > 4.0) m1 = 0.9745;
    if(f2 > 4.0) m2 = 0.9852;
    if(f3 > 4.0) m3 = 0.9975;
    
    if(f1 < 0.25) m1 = 1.0754;
    if(f2 < 0.25) m2 = 1.0573;
    if(f3 < 0.25) m3 = 1.0386;
      
    if(millis() > 10000) {
      Music.setGain(gain);
      gain *= 0.999;
    }
    

}

