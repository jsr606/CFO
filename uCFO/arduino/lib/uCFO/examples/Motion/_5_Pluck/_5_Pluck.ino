//Pluck: one pluck at x=512

int x, fout, count;
float f;
float w = 50; //width of pluck
float h = 500; //height of pluck
float slope = h/w;
boolean forward = true;

#include <Motor.h>

void setup(){
  MotorA.init();
  MotorB.init();
  Serial.begin(9600);
}

void loop(){
  x = analogRead(A0) - 512;
  if (forward){
    if (x <= - w/2) f = 0;
    if (x > -w/2 && x< w/2) f = - slope*(x + w/2);
    if (x > w/2){
      f = 0;
      forward = false;
      Serial.println("pluck forward");
    }
  }
  else
  {
    if (x > w/2) f = 0;
    if (x < w/2 && x > -w/2) f = - slope*(x - w/2);
    if (x < -w/2){
      f = 0;
      forward = true;
      Serial.println("pluck back");
    }
  }
  fout = int(f);
  MotorA.torque(fout);
  
  if(count++>=0){
    count=-1000; // wait 1000 loops before print
    Serial.print(x,DEC);
    Serial.print(" ");
    Serial.println(fout,DEC);
  }
}
