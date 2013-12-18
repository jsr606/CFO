
#include <Motion.h>
#include <Motor.h>

#define NEW '!'
#define OK '*'
#define PRINT '&'

int table[800];
int len = 800;

char buf[32] = "";
int cnt = 0;

void setup() {
  Serial.begin(9600);
  MotorA.init();
  MotionA.init(INPUTA0);
}

void loop() {  
  MotionA.update_position();
  int i = (int) (((float) MotionA.Xin / 1023.0) * len);
  int f = table[i];
  MotorA.torque(f);
  
  // TODO - send position back to interface
  /* 
  cnt++;
  if(cnt == 1000) {
    sprintf(buf, "%d %d %d", MotionA.Xin, i, f);
    Serial.println(buf);
    cnt = 0;
  }
  */
}


void serialEvent() {
  
  if(Serial.available()) {
    char in = (char)Serial.read();
    if(in == NEW) {
      Serial.print(OK);
      len = serial_read();
      for(int i = 0; i < len; i++) {
        table[i] = serial_read();  
      }
    } else if(in == PRINT) {
      Serial.println(len);
      for(int i = 0; i < len; i++) {        
        Serial.print(table[i]);
        Serial.print(';');
      }
      Serial.print('#');
    }
  }  
}


int serial_read() {
  while(Serial.available() < 2);
  int l = Serial.read();
  int h = Serial.read();
  Serial.write(OK);
  return (h * 256) + l;          
}
