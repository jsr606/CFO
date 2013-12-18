// Center - both motors A and B
// feels like "Double Toggle"!

#include <Motor.h>

int duty, count, fout;
int xA, xB, foutA, foutB;

void setup(){
  Serial.begin(9600);
  MotorA.init();
  MotorB.init();
}

void loop(){
   
  xA = analogRead(A0); 
  xB = analogRead(A3);
  foutA = 6*(xB-512); // this will peak at x=1024/6
  MotorA.torque(foutA);  // 1/4 or 1/2 ?

  foutB = 6*(xA-512); // this will peak at x=1024/6
  MotorB.torque(foutB);  // 1/4 or 1/2 ?

}
