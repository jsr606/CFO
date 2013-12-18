// Theremin
// A: pitch, B: volume

#include <Music.h>
int xA, xB; //10bit A/D gives 0-1023
float p; //midi note number 0-127
double frequency, volume;
byte c; //print every 256 cycles

void setup(){
  Music.init();
  Serial.begin(9600);
}
void loop(){
  xA = analogRead(A3); // A position
  p = float(xA)/8; //pitch (midi)
  
  p = 80.*(p/128.)+20.;
  // p = map(p, 0,127,20,100);
  //linear mapping of p
  //The map() function uses integer math so will
  //not generate fractions, when the math might
  //indicate that it should do so. Fractional
  //remainders are truncated, and are not rounded 
  //or averaged. 
  //
  frequency = pow(2,((p-69.)/12.))*440; //midi 69 -> 440hz
  Music.setFrequency(frequency);
  
  xB = analogRead(A0);
  volume = -float(xB)/1024.; // B position
  volume = pow(10,volume);
  Music.setGain(volume);
  
}
  
