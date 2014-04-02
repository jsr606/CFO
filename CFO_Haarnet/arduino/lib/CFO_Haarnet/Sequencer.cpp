/*
 Haarnet.h - Friction Music library
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

#include "Sequencer.h"
#include <Arduino.h>

MSequencer Sequencer;

void MSequencer::init()
{
    for(int i = 0; i < MAX_SEQ; i++) {
        _sequences[i] = NULL;
    }
}

void MSequencer::update()
{
    for(int i = 0; i < MAX_SEQ; i++) {
        seq* s = _sequences[i];
        if(s == NULL || s->_stoped) continue;
        
        unsigned long tick = millis();
        if(tick - s->ltick > s->_tempo) {
            //boom!
            s->_callback();
            s->ltick = millis();
        }
    }
}

int MSequencer::newSequence(int bpm, func_cb cb)
{
    int j = -1;
    for(int i = 0; i < MAX_SEQ; i++) {
        if(_sequences[i] == NULL) j = i;
    }
    
    if(j >= 0) {
        seq* s = new seq(bpm, cb);
        _sequences[j] = s;
    }
    
    return j;
}

bool MSequencer::stopSequence(int index)
{
    if(index >= 0 && index < MAX_SEQ) {
        _sequences[index]->_stoped = true;
        return true;
    }
    return false;
}

bool MSequencer::startSequence(int index)
{
    if(index >= 0 && index < MAX_SEQ) {
        _sequences[index]->_stoped = false;
        _sequences[index]->ltick = millis();
        return true;
    }
    return false;
}

// seq

seq::seq(int bpm, func_cb cb) : _stoped(true)
{
    setbpm(bpm);
    callback(cb);
}

void seq::setbpm(int v)
{
    _bpm = v;
    _tempo = int( 60000.0 / (float)v );
}

int seq::getbpm()
{
    return _bpm;
}

void seq::callback(func_cb cb)
{
    _callback = cb;
}




