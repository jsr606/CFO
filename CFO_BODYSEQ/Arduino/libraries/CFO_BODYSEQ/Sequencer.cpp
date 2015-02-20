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
#include "BodyseqSynth.h"
#include <spi4teensy3.h>
#include <Arduino.h>

MSequencer Sequencer;


void MSequencer::init(int bpm)
{
    setbpm(bpm);
    clockTick = 0;
    for(int i = 0; i < MAX_SEQ; i++) {
        _sequences[i] = NULL;
    }
}


void MSequencer::internalClock()
{
    timeNow = micros();
    if(timeNow - lastTime >= tickTime) {
        clock();
        Midi.sendClock();
        lastTime = timeNow;
    }
}


void MSequencer::update()
{
    if(getInternalClock()) internalClock();
    
    for(int i = 0; i < MAX_SEQ; i++) {
        seq* s = _sequences[i];
        if(s == NULL || s->_stopped) continue;
        if(clockTick >= s -> step) {
            if(s -> _steps) {
                s -> trigger();
            } else {
                s->_callback();
            }
            s->step += s -> _subdiv;
        }
    }
}


void MSequencer::setInternalClock(bool i)
{
    _internalClock = i;
}


bool MSequencer::getInternalClock()
{
    return _internalClock;
}


void MSequencer::clock()
{
    clockTick++;
}


void MSequencer::start()
{
    clockTick = 0;
    for(int i = 0; i < MAX_SEQ; i++) {
        startSequence(i);
    }
    Midi.sendStart();
}


void MSequencer::continues()
{
    clockTick = 0;  // is this right????
    for(int i = 0; i < MAX_SEQ; i++) {
        continueSequence(i);
    }
    Midi.sendContinue();
}


void MSequencer::stop()
{
    for(int i = 0; i < MAX_SEQ; i++) {
        stopSequence(i);
    }
    Midi.sendStop();
}


int MSequencer::newSequence(SUBDIV subdiv, func_cb cb)
{
    int j = -1;
    for(int i = 0; i < MAX_SEQ; i++) {
        if(_sequences[i] == NULL) {
            j = i;
            break;
        }
    }
    
    if(j >= 0) {
        seq* s = new seq(j, subdiv, cb);
        _sequences[j] = s;
        Serial.print("Created sequence ");
        Serial.println(j);
    }
    
    return j;
}


int MSequencer::newSequence(SUBDIV subdiv, int steps, SEQ_LOOP_TYPE loop)
{
    int j = -1;
    for(int i = 0; i < MAX_SEQ; i++) {
        if(_sequences[i] == NULL) {
            j = i;
            break;
        }
    }
    
    if(j >= 0) {
        seq* s = new seq(j, subdiv, steps, loop);
        _sequences[j] = s;
        Serial.print("Created sequence ");
        Serial.println(j);
    }
    
    return j;
}


int MSequencer::newSequence(SUBDIV subdiv, int steps, int channel)
{
    int j = -1;
    for(int i = 0; i < MAX_SEQ; i++) {
        if(_sequences[i] == NULL) {
            j = i;
            break;
        }
    }
    
    if(j >= 0) {
        seq* s = new seq(j, subdiv, steps, channel);
        _sequences[j] = s;
        Serial.print("Created sequence ");
        Serial.println(j);
    }
    
    return j;
}


int MSequencer::newSequence(SUBDIV subdiv, int steps, SEQ_LOOP_TYPE loop, bool reverse)
{
    int j = -1;
    for(int i = 0; i < MAX_SEQ; i++) {
        if(_sequences[i] == NULL) {
            j = i;
            break;
        }
    }
    
    if(j >= 0) {
        seq* s = new seq(j, subdiv, steps, loop, reverse);
        _sequences[j] = s;
        Serial.print("Created sequence ");
        Serial.println(j);
    }
    
    return j;
}


bool MSequencer::stopSequence(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->_stopped = true;
        return true;
    }
    return false;
}


bool MSequencer::startSequence(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index] -> _stopped = false;
        _sequences[index] -> step = 0;
        return true;
    }
    return false;
}


bool MSequencer::continueSequence(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->_stopped = false;
        return true;
    }
    return false;
}


void MSequencer::setbpm(int bpm)
{
    if(bpm > 0){
        _bpm = bpm;
        _bpmInClockTicks = _bpm * 24;
        tickTime = 60 * 1000000 / _bpmInClockTicks;
    }
}

int MSequencer::getbpm()
{
    return _bpm;
}


bool MSequencer::setChannel(int index, int channel)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->_channel;
//        _sequences[index]->setchannel(channel);
        return true;
    }
    return false;
}


int MSequencer::getChannel(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->_channel;
//        return _sequences[index]->getchannel();
    }
    return -1;
}


bool MSequencer::setSteps(int index, int steps)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setsteps(steps);
        return true;
    }
    return false;
}


int MSequencer::getSteps(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getsteps();
    }
    return -1;
}


bool MSequencer::setPosition(int index, int position)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setposition(position);
        return true;
    }
    return false;
}


int MSequencer::getPosition(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getposition();
    }
    return -1;
}


bool MSequencer::setBegin(int index, int begin)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setbegin(begin);
        return true;
    }
    return false;
}


int MSequencer::getBegin(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getbegin();
    }
    return -1;
}


bool MSequencer::setEnd(int index, int end)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setend(end);
        return true;
    }
    return false;
}


int MSequencer::getEnd(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getend();
    }
    return -1;
}


bool MSequencer::setReverse(int index, bool reverse)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setreverse(reverse);
        return true;
    }
    return false;
}


bool MSequencer::getReverse(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getreverse();
    }
    return -1;
}


bool MSequencer::setInternal(int index, bool internal)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setinternal(internal);
        return true;
    }
    return false;
}


bool MSequencer::getInternal(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getinternal();
    }
    return -1;
}


bool MSequencer::setExternal(int index, bool external)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setexternal(external);
        return true;
    }
    return false;
}


bool MSequencer::getExternal(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getexternal();
    }
    return -1;
}


bool MSequencer::setSubdiv(int index, SUBDIV subdiv)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setsubdiv(subdiv);
        return true;
    }
    return false;
}


int MSequencer::getSubdiv(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getsubdiv();
    }
    return -1;
}


bool MSequencer::setLoopType(int index, SEQ_LOOP_TYPE loop)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setlooptype(loop);
        return true;
    }
    return false;
}


int MSequencer::getLoopType(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getlooptype();
    }
    return -1;
}


bool MSequencer::setCallback(int index, func_cb cb)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->callback(cb);
        return true;
    }
    return false;
}


func_cb MSequencer::getCallback(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->_callback;
    }
    return NULL;
}



bool MSequencer::insertNotes(int index, int notes[], int numNotes, int newPosition)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->insertnotes(notes, numNotes, newPosition);
        return true;
    }
    return false;
}



// seq

seq::seq(int id, SUBDIV subdiv, func_cb cb) : _id(id), _stopped(true)
{
    setsubdiv(subdiv);
    callback(cb);
}


seq::seq(int id, SUBDIV subdiv,  int steps, int channel) : _id(id), _stopped(true)
{
    setchannel(channel);
    setsubdiv(subdiv);
    setsteps(steps);
    setlooptype(LOOP);
    setreverse(false);
    setposition(0);
    setbegin(0);
    if(steps <= MAX_STEPS) setend(steps-1);
    else setend(MAX_STEPS);
    for(int i = 0; i < MAX_STEPS; i++) {
        _notes[i] = 36 + 3 * i;
        _velocity[i] = 127;
    }
    setinternal(true);
    setexternal(true);
    _lastposition = 0;
    
}


seq::seq(int id, SUBDIV subdiv,  int steps, SEQ_LOOP_TYPE loop) : _id(id), _stopped(true)
{
    setchannel(Midi.midiChannel);
    setsubdiv(subdiv);
    setsteps(steps);
    setlooptype(loop);
    setreverse(false);
    setposition(0);
    setbegin(0);
    if(steps <= MAX_STEPS) setend(steps-1);
    else setend(MAX_STEPS);
    for(int i = 0; i < MAX_STEPS; i++) {
        _notes[i] = 36 + 3 * i;
        _velocity[i] = 127;
    }
    setinternal(true);
    setexternal(true);
    _lastposition = 0;
}


seq::seq(int id, SUBDIV subdiv,  int steps, SEQ_LOOP_TYPE loop, bool reverse) : _id(id), _stopped(true)
{
    setchannel(Midi.midiChannel);
    setsubdiv(subdiv);
    setsteps(steps);
    setlooptype(loop);
    if(reverse) {
        setreverse(true);
        setposition(steps-1);
    } else {
        setreverse(false);
        setposition(0);
    }
    setbegin(0);
    if(steps <= MAX_STEPS) setend(steps-1);
    else setend(MAX_STEPS);
    for(int i = 0; i < MAX_STEPS; i++) {
        _notes[i] = 36 + 3 * i;
        _velocity[i] = 127;
    }
    setinternal(true);
    setexternal(true);
    _lastposition = 0;
}


void seq::trigger()
{
    if(_internal) {
        Midi.noteOff(_channel, _notes[_lastposition], _velocity[_lastposition]);
        Midi.noteOn(_channel, _notes[_position], _velocity[_position]);
    }
    if(_external) {
        Midi.sendNoteOff(_channel, _notes[_lastposition], _velocity[_lastposition]);
        Midi.sendNoteOn(_channel, _notes[_position], _velocity[_position]);
    }
    
    _lastposition = _position;

    if(_reverse) {
        if(_position <= _begin) {
            _position = _end + 1;
            if(!_loop) _stopped = true;
        }
        _position--;
    } else {
        if(_position >= _end) {
            _position = _begin - 1;
            if(!_loop) _stopped = true;
        }
        _lastposition = _position;
        _position++;
    }
//    Serial.println("triggered");
}


void seq::insertnotes(int notes[], int numNotes, int newPosition)
{
    for(int i = 0; i < numNotes; i++) {
        int pos = newPosition + i;
        int note = notes[i];
        if(note > 127) note = 127;
        else if(note < 0 ) note = 0;
        if((pos >= 0) && (pos < MAX_STEPS)) {
            _notes[pos] = note;
        }
    }
}


void seq::setchannel(int c)
{
    _channel = c;
}


int seq::getchannel()
{
    return _channel;
}


void seq::setsteps(int s)
{
    _steps = s;
}


int seq::getsteps()
{
    return _steps;
}


void seq::setposition(int p)
{
    _position = p;
}


int seq::getposition()
{
    return _position;
}


void seq::setbegin(int b)
{
    if(b < 0) b = 0;
    if(b > _end) b = _end;
    if(b >= _steps) b = _steps - 1;
    _begin = b;
}


int seq::getbegin()
{
    return _begin;
}


void seq::setend(int e)
{
    if(e < 0) e = 0;
    if(e >= _steps) e = _steps - 1;
    if(e < _begin) e = _begin;
    _end = e;
}


int seq::getend()
{
    return _end;
}


void seq::setreverse(bool r)
{
    _reverse = r;
}


bool seq::getreverse()
{
    return _reverse;
}


void seq::setinternal(bool i)
{
    _internal = i;
}


bool seq::getinternal()
{
    return _internal;
}


void seq::setexternal(bool e)
{
    _external = e;
}


bool seq::getexternal()
{
    return _external;
}


void seq::setsubdiv(SUBDIV v)
{
    _subdiv = v;
}


SUBDIV seq::getsubdiv()
{
    return _subdiv;
}


void seq::setlooptype(SEQ_LOOP_TYPE loop)
{
    _loop = loop;
}


SEQ_LOOP_TYPE seq::getlooptype()
{
    return _loop;
}


void seq::callback(func_cb cb)
{
    _callback = cb;
}