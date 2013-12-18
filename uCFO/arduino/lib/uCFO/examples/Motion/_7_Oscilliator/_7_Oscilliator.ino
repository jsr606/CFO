/*
This sketch runs with Processing sketch Oscilloscope.pde
*/

#include <Motion.h>
#include <Motor.h>
#include <Music.h>

char buf[16] = "";

char b = 'x';
String  inputString = "";
boolean stringComplete = false;

float k, m, d;

int cnt = 0;

void setup() {
  
  // MOTOR
  MotorA.init();
  
  // MUSIC 
  Music.init();
  
  // MOTION
  MotionA.init(INPUTA0);
  MotionA.k = 5.2f;   // spring
  MotionA.m = 1.0f;   // mass
  MotionA.d = 8.02f;  // damping

  // Serial
  Serial.begin(9600);  
}

void loop() {
  
  MotionA.update_mass_spring_damper();
  
  MotorA.torque(MotionA.F);
  
  int f = map(abs(MotionA.F), 0, 512, 64, 76);
  Music.noteOn(f);
  
  cnt++;
  
  if(cnt == 10) {  
    sprintf(buf, "%d %d %d %d", (int)MotionA.F, (int)MotionA.V, (int)MotionA.X, (int)MotionA.Xin);
    Serial.println(buf);
    cnt = 0;
  }  
  
  if(stringComplete) {
    if(b == 'k') {
      MotionA.k = convertToFloat(inputString);
    } else if(b == 'm') {
      MotionA.m = convertToFloat(inputString);
    } else if(b == 'd') {
      MotionA.d = convertToFloat(inputString);
    }    
    b = 'x';
    stringComplete = false;
    inputString = "";
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();     
    if(inChar == 'k' || inChar == 'm' || inChar == 'd') {
      b = inChar;
    } else {
      if (inChar == ';') {
        stringComplete = true;
      } else       
        inputString += inChar;
    }
  }
}

float convertToFloat(String str) {
    char buf[16] = "";
    str.toCharArray(buf, str.length() + 1);
    return atof(buf);  
}
