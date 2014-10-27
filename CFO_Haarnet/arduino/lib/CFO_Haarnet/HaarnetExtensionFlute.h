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

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

// define i/o pin
const int pin_slider		= 4;
const int pin_LED_A         = 9;
const int pin_LED_B         = 5;
const int pin_push_button	= 6;
const int pin_bodyswitch_A 	= A6;
const int pin_bodyswitch_B 	= A3;
const int pin_bodyswitch_C 	= A7;
const int pin_lux_0 		= A0;
const int pin_lux_1 		= A1;
const int pin_mic           = A8;

class FluteExt {
public:
    
    FluteExt();
    
    void init();
    
    int sample_mic(int nsamples);
    
    int touchA();
    int touchB();
    int touchC();
    
    void RGB(int* rgbc);
    
    int luxA();
    int luxB();
    
    bool push_button();
    bool switch_position();
    
    void ledA(int v);
    void ledB(int v);
    
    
protected:
    
    uint16_t _r, _g, _b, _c;
    int _mic, _tA, _tB, _tC, _luxA, _luxB, _ledA, _ledB;
    bool _pb, _slider;
    Adafruit_TCS34725 tcs;
    
    
};

extern FluteExt FluteEx;


