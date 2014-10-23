#include <Wire.h>
#include <Adafruit_TCS34725.h>

 // define i/o pin
const int pin_slider		= 4;
const int pin_LED_A		= 9;
const int pin_LED_B		= 5;
const int pin_push_button	= 6;
const int pin_bodyswitch_A 	= A6;
const int pin_bodyswitch_B 	= A3;
const int pin_bodyswitch_C 	= A7;
const int pin_lux_0 		= A0;
const int pin_lux_1 		= A1;
const int pin_mic 		= A8;

uint16_t _r, _g, _b, _c;
int _mic, _tA, _tB, _tC, _luxA, _luxB, _ledA, _ledB;
bool _pb, _slider;


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void init_extension() {
	
	pinMode(pin_slider, INPUT_PULLUP);
	pinMode(pin_push_button, INPUT_PULLUP);
	pinMode(pin_LED_A, OUTPUT);
	pinMode(pin_LED_B, OUTPUT);
	pinMode(pin_lux_0, INPUT);
	pinMode(pin_lux_1, INPUT);
	pinMode(pin_mic, INPUT);

	// set internal pulldown resistors on bodyswitches
	*portConfigRegister(pin_bodyswitch_A) = PORT_PCR_MUX(1) | PORT_PCR_PE;
	*portConfigRegister(pin_bodyswitch_B) = PORT_PCR_MUX(1) | PORT_PCR_PE;
	*portConfigRegister(pin_bodyswitch_C) = PORT_PCR_MUX(1) | PORT_PCR_PE;

	tcs.begin();
}

 int sample_mic(int samples) {

  // we are feeding the mic 5V, but sampling it to 3,3V
  // audio waves are centered around 2,5V (not 3,3V/2)
  // this means readings are centered around 1023*2,5/3,3=775, not 512 which would be the normal
  
  int totalVol = 0;
  int samplesDone = 0;
  while (samplesDone < samples) {
    
    int thisReading = analogRead(pin_mic)-775;
    
    if (thisReading < 0) {
      totalVol = totalVol + abs(thisReading);
      samplesDone++;
    }
    delayMicroseconds(5);
  }

  _mic = totalVol/samples;

  return _mic;

 }

 int touchA() {
 	_tA = analogRead(pin_bodyswitch_A);
 	return _tA;
 }

int touchB() {
 	_tB = analogRead(pin_bodyswitch_B);
 	return _tB;
}

int touchC() {
 	_tC = analogRead(pin_bodyswitch_C);
 	return _tC;
}


void RGB(int* rgbc) {

	tcs.setInterrupt(false);      // turn on LED
	delay(60);  // takes 50ms to read 
	tcs.getRawData(&_r, &_g, &_b, &_c);
	tcs.setInterrupt(true);  // turn off LED

	rgbc[0] = _r;
	rgbc[1] = _g;
	rgbc[2] = _b;
	rgbc[3] = _c;
}

int luxA() {
	_luxA = analogRead(pin_lux_0);
	return _luxA;
}

int luxB() {
	_luxB = analogRead(pin_lux_1);
	return _luxB;
}


bool push_button() {
	_pb = !digitalRead(pin_push_button);
	return _pb;
}

bool slider() {
	_slider = digitalRead(pin_slider);
	return _slider;
}

void ledA(int v) {
	_ledA = constrain(v, 0, 255);
	analogWrite(pin_LED_A, _ledA);
}

void ledB(int v) {
	_ledB = constrain(v, 0, 255);
	analogWrite(pin_LED_B, _ledB);
}




