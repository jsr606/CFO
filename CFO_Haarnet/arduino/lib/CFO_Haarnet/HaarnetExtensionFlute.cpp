/*
 HaarnetExtensionFlute.h - Friction Music library
 Copyright (c) 2014 Science Friction.
 All right reserved.
 
 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your optionosc1modShape_ptr) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser Public License for more details.
 
 You should have received a copy of the GNU Lesser Public License
 along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 + author: gauthiier
 + contact: d@gauthiier.info
 */

#include "HaarnetExtensionFlute.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

FluteExt FluteEx;


FluteExt::FluteExt() {
    
}

void FluteExt::init(){
    
    tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
    
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

int FluteExt::sample_mic(int nsamples){
    
    // we are feeding the mic 5V, but sampling it to 3,3V
    // audio waves are centered around 2,5V (not 3,3V/2)
    // this means readings are centered around 1023*2,5/3,3=775, not 512 which would be the normal
    
    int totalVol = 0;
    int samplesDone = 0;
    while (samplesDone < nsamples) {
        
        int thisReading = analogRead(pin_mic)-775;
        
        if (thisReading < 0) {
            totalVol = totalVol + abs(thisReading);
            samplesDone++;
        }
        //delayMicroseconds(5);
    }
    
    _mic = totalVol/nsamples;
    
    return _mic;
    
}

int FluteExt::touchA(){
    _tA = analogRead(pin_bodyswitch_A);
    return _tA;
}

int FluteExt::touchB(){
    _tB = analogRead(pin_bodyswitch_B);
    return _tB;
}

int FluteExt::touchC(){
    _tC = analogRead(pin_bodyswitch_C);
    return _tC;
}

void FluteExt::RGB(int* rgbc) {
    
    tcs.setInterrupt(false);      // turn on LED
    delay(60);  // takes 50ms to read
    tcs.getRawData(&_r, &_g, &_b, &_c);
    tcs.setInterrupt(true);  // turn off LED
    
    rgbc[0] = _r;
    rgbc[1] = _g;
    rgbc[2] = _b;
    rgbc[3] = _c;
}

int FluteExt::luxA() {
    _luxA = analogRead(pin_lux_0);
    return _luxA;
}

int FluteExt::luxB() {
    _luxB = analogRead(pin_lux_1);
    return _luxB;
}


bool FluteExt::push_button() {
    _pb = !digitalRead(pin_push_button);
    return _pb;
}

bool FluteExt::switch_position() {
    _slider = digitalRead(pin_slider);
    return _slider;
}

void FluteExt::ledA(int v) {
    _ledA = constrain(v, 0, 255);
    analogWrite(pin_LED_A, _ledA);
}

void FluteExt::ledB(int v) {
    _ledB = constrain(v, 0, 255);
    analogWrite(pin_LED_B, _ledB);
}
