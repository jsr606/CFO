#include <EEPROM.h>
#include <spi4teensy3.h>
#include <Mcp4251.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

#include <HaarnetExtensionFlute.h>

int rgbc[4];

void setup() {  
  // Inititalise Flute
  FluteEx.init();  
}

void loop() {
  
  
  Serial.print("Switch: "); Serial.println((FluteEx.switch_position() == true ? "A" : "B"));
  Serial.print("PushButton: "); Serial.println((FluteEx.push_button() == true ? "ON" : "OFF"));
  
  Serial.print("Touch A: "); Serial.println(FluteEx.touchA());
  Serial.print("Touch B: "); Serial.println(FluteEx.touchB());
  Serial.print("Touch C: "); Serial.println(FluteEx.touchC());
  
  FluteEx.RGB(rgbc);
  Serial.print("R: "); Serial.println(rgbc[0]);
  Serial.print("G: "); Serial.println(rgbc[1]);
  Serial.print("B: "); Serial.println(rgbc[2]);
  Serial.print("C: "); Serial.println(rgbc[3]);
  
  Serial.print("Lux B: "); Serial.println(FluteEx.luxB());
  
  Serial.print("Mic: "); Serial.println(FluteEx.sample_mic(50));
  
  Serial.println("....");
  
  delay(150);
  
}


