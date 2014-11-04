#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Haarnet.h>

// adafruit RGB sensor setup
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
byte gammatable[256];
uint16_t clear, red, green, blue;

// define input pins
const int slideSwitch=4;
const int LED1=9;
const int pushButton=6;
const int LED2=5;
const int bodySwitch1 = A6;
const int bodySwitch2 = A3;
const int bodySwitch3 = A7;
//const int pot1 = A0;
const int microphone = A8;
const int lightSensor = A1;

// add pots
const int pot1 = A0;
const int pot2 = A2;
const int pot3 = A9;

// mic averaging
float peakLevel = 0;

// debug stuff
boolean debug = false;
boolean graphing = false;

void setup() {
  // internal pullups
  pinMode(slideSwitch, INPUT_PULLUP);
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  // set internal pulldown resistors on bodyswitches
  *portConfigRegister(bodySwitch1) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  *portConfigRegister(bodySwitch2) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  *portConfigRegister(bodySwitch3) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  
  if (tcs.begin()) Serial.println("found RGB color sensor");
  
  Music.init();
  //disableEnvelope1();
  //disableEnvelope2();
  Music.getPreset(16);
  Music.noteOn(48,127);
}

void loop() {

  // read analog inputs
  int bs1 = analogRead(bodySwitch1);
  int bs2 = analogRead(bodySwitch2);
  int bs3 = analogRead(bodySwitch3);
  int lightLevel = analogRead(lightSensor);
  // int pot = analogRead(pot1);

  // read color sensor
  colorSensor();
  
  // read microphone
  int mic = sampleMic(20);
  peakLevel = max(peakLevel, mic);
  peakLevel = (peakLevel * 0.99); // ease out peak level nicely

  // update LEDs
  int lightLevel1 = map(mic,0,1000,0,255);
  lightLevel1 = constrain(lightLevel1,0,255);
  int lightLevel2 = map(peakLevel,0,1000,0,255);
  lightLevel2 = constrain(lightLevel2,0,255);
  analogWrite(LED1, lightLevel);
  analogWrite(LED2, 0);
  
  int p1 = analogRead(pot1);
  
  float gain = map(pot1,0,1023,0,1000)/1000;
  setGain(gain); // 0.0 - 1.0
  
  int p2 = analogRead(pot2);
  int p3 = analogRead(pot3);
  Serial.print(p1);
  Serial.print(",");
  Serial.print(p2);
  Serial.print(",");
  Serial.println(p3);
  
  if (graphing) {
    Serial.print(bs1);
    Serial.print(",");
    Serial.print(bs2);
    Serial.print(",");
    Serial.print(bs3);
    Serial.print(",");
    Serial.print(mic);
    Serial.print(",");
    Serial.println(lightLevel);
  }
  
  boolean slide = digitalRead(slideSwitch);
  boolean push = !digitalRead(pushButton);
  
  if (push) Serial.println("push!");
  if (slide) Serial.println("slide!");

  if (debug) {
    Serial.print(bs1);  
    Serial.print("\t");
    Serial.print(bs2);  
    Serial.print("\t");
    Serial.print(bs3);
    Serial.print("\t");
    Serial.print(lightLevel);
    Serial.println("\tmin: ");
  }
  
  delay(10);
}

int sampleMic(int samples) {
  
  // we are feeding the mic 5V, but sampling it to 3,3V
  // audio waves are centered around 2,5V (not 3,3V/2)
  // this means readings are centered around 1023*2,5/3,3=775, not 512 which would be the normal
  
  long totalVol = 0;
  int samplesDone = 0;
  while (samplesDone < samples) {
    
    int thisReading = analogRead(microphone)-775;
    
    if (thisReading < 0) {
      totalVol = totalVol + abs(thisReading);
      samplesDone++;
    }
    delayMicroseconds(5);
  }
  return (totalVol/samples);
}

void colorSensor() {
  
  tcs.setInterrupt(false);      // turn on LED
  delay(60);  // takes 50ms to read 
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true);  // turn off LED
  
  if (debug) {
    Serial.print("C:\t"); Serial.print(clear);
    Serial.print("\tR:\t"); Serial.print(red);
    Serial.print("\tG:\t"); Serial.print(green);
    Serial.print("\tB:\t"); Serial.print(blue);
    Serial.println();
  }
  
  // Figure out some basic hex code for visualization
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  
  if (debug) {
    Serial.print("\t");
    Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
    Serial.println();
  }
}
