/*
 Sampler.cpp - Friction Music library
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

#include "Sampler.h"
#include <Arduino.h>


Sampler::Sampler(int len) {
    _samples = (int*)malloc(len * 2);
    _len = len;
    memset(_samples, 0, len * 2);
}

void Sampler::record(int in) {
    _samples[_indx++] = in;
    if(_indx > _len) _indx = 0;
}

int Sampler::next() {
    if(_indx > _len) _indx = 0;
    return _samples[_indx++];
}

void Sampler::reset() {
    _indx = 0;
}