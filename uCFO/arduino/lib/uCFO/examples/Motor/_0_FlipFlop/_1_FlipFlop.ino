#include <MotorT3.h>

void setup() {
  MotorA.init();
}

void loop() {
  MotorA.torque(500);
  delay(2000);
  MotorA.torque(-500);
  delay(2000);
}
