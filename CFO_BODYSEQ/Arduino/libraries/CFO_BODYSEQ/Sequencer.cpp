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
#include <spi4teensy3.h>
#include <Arduino.h>

MSequencer Sequencer;
iSequencer iSeq; // instrument sequencer
//extern iSeq;
//int iSeq0,iSeq1;
//iSeq.iSeqInit(120);

IntervalTimer sequencerTimer;
IntervalTimer iSeqTimer;

boolean sequencerTimerRunning = false;
boolean iSeqTimerRunning = false;

#ifndef SAMPLE_RATE
unsigned int sampleRate = 48000;
#endif

void sequencer_isr(void)
{
    Sequencer.clockStep++;
}

void iSeq_isr(void)
{
    iSeq.clockStep++;
}


void MSequencer::init(int bpm)
{
    setbpm(bpm);
    for(int i = 0; i < MAX_SEQ; i++) {
        _sequences[i] = NULL;
    }
    if(!sequencerTimerRunning) {
        sequencerTimerRunning = true;
        clockStep = 0;
        sequencerTimer.begin(sequencer_isr, 60 * 1000000 / (_bpm * TICKS_PER_QUARTER_NOTE));
    }
}


void iSequencer::init(int bpm)
{
    for(int i = 0; i < ISEQ_NBR_STEPS; i++) {
        //        _notes[i] = i;
        //        _velocity[i] = 127;
        //        _CCnumbers[i] = 100;
        //        _CCvalues[i] = 0;
    }

    

//
//    int iSeq_indx0 = 0;
//    int iSeq_indx1 = 0;
//#define ISEQ_NBR_STEPS 32
//    const int iSeq_nbr_notes = 32;
//    const int iSeq_nbr_steps = 32;
//    
//    int iSeq_notes[ISEQ_NBR_STEPS];
//    int iSeq_velocity[ISEQ_NBR_STEPS];
//    int iSeq_midi_numbers[ISEQ_NBR_STEPS];
//    int iSeq_midi_values[ISEQ_NBR_STEPS];
//    
//    void iSeqNote() {
//        //    Music.noteOn(iSeq_notes[iSeq_indx0++] + Music.notePlayed, Music.velocityPlayed);
//        //    if(iSeq_indx0 >= iSeq_nbr_notes) iSeq_indx0 = 0;
//    }
//    
//    void iSeqController() {
//        //    Midi.controller(MIDI_CHANNEL - 1, iSeq_midi_numbers[iSeq_indx1], iSeq_midi_values[iSeq_indx1]);
//        //    iSeq_indx1++;
//        //    if(iSeq_indx1 >= iSeq_nbr_steps) iSeq_indx1 = 0;
//    }
//    
//    for(int i = 0; i < ISEQ_NBR_STEPS; i++) {
//        //        iSeq_notes[i] = i;
//        //        iSeq_velocity[i] = 127;
//        //        iSeq_midi_numbers[i] = 100;
//        //        iSeq_midi_values[i] = 0;
//    }
    
    setbpm(bpm);
    for(int i = 0; i < INSTR_SEQ; i++) {
        _sequences[i] = NULL;
    }
    if(!iSeqTimerRunning) {
        iSeqTimerRunning = true;
        clockStep = 0;
        iSeqTimer.begin(iSeq_isr, 60 * 1000000 / (_bpm * TICKS_PER_QUARTER_NOTE));
    }
}



void MSequencer::update()
{
    for(int i = 0; i < MAX_SEQ; i++) {
        seq* s = _sequences[i];
        if(s == NULL || s->_stopped) continue;
        if(clockStep >= s -> step) {
            //boom!
            s->_callback(); // add to queue???
            s->step += s -> _subdiv;
//            Serial.println(s -> step);
        }
    }
    // queue goes here
}


void iSequencer::update()
{
    for(int i = 0; i < INSTR_SEQ; i++) {
        iseq* s = _sequences[i];
        if(s == NULL || s->_stopped) continue;
        if(clockStep >= s -> step) {
            //boom!
//            s->_callback(); // add to queue???
            s->step += s -> _subdiv;
//            Serial.println(s -> step);
        }
    }
    // queue goes here
}


int MSequencer::newSequence(func_cb cb, SUBDIV subdiv)
{
    int j = -1;
    for(int i = 0; i < MAX_SEQ; i++) {
        if(_sequences[i] == NULL) j = i;
    }
    
    if(j >= 0) {
        seq* s = new seq(j, cb, subdiv);
        _sequences[j] = s;
        Serial.print("Created sequence ");
        Serial.println(j);
    }
    
    return j;
}


int iSequencer::newSequence(SUBDIV subdiv, int steps, SEQ_LOOP_TYPE loop)
{
    int j = -1;
    for(int i = 0; i < MAX_SEQ; i++) {
        if(_sequences[i] == NULL) j = i;
    }
    
    if(j >= 0) {
        iseq* s = new iseq(j, subdiv, steps, loop);
        _sequences[j] = s;
        Serial.print("Created sequence ");
        Serial.println(j);
    }
    
    return j;
}


bool MSequencer::stopSequence(int index)
{
    if(index >= 0 && index < MAX_SEQ) {
        _sequences[index]->_stopped = true;
//        _sequences[index]-> step = 0;
        return true;
    }
    return false;
}

bool MSequencer::startSequence(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->_stopped = false;
        _sequences[index]-> step = 0;
        return true;
    }
    return false;
}


void MSequencer::setbpm(int bpm)
{
    _bpm = bpm;
    _bpmInClockSteps = _bpm * 24;
}

int MSequencer::getbpm()
{
    return _bpm;
}

void iSequencer::setbpm(int bpm)
{
    _bpm = bpm;
    _bpmInClockSteps = _bpm * 24;
}

int iSequencer::getbpm()
{
    return _bpm;
}

bool MSequencer::setSequenceSubdiv(int index, SUBDIV subdiv)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        _sequences[index]->setsubdiv(subdiv);
        return true;
    }
    return false;
}

int MSequencer::getSequenceSubdiv(int index)
{
    if(index >= 0 && index < MAX_SEQ && _sequences[index] != NULL) {
        return _sequences[index]->getsubdiv();
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


// seq

seq::seq(int id, func_cb cb, SUBDIV subdiv) : _id(id), _stopped(true)
{
    setsubdiv(subdiv);
    callback(cb);
}

void seq::setsubdiv(SUBDIV v)
{
    _subdiv = v;
    
}

SUBDIV seq::getsubdiv()
{
    return _subdiv;
}

void seq::callback(func_cb cb)
{
    _callback = cb;
}

// iseq

iseq::iseq(int id, SUBDIV subdiv, int steps, SEQ_LOOP_TYPE loop) : _id(id), _stopped(true)
{
    setsubdiv(subdiv);
    setsteps(steps);
    setlooptype(loop);
}

void iseq::setsteps(int steps)
{
    _steps = steps;
    
}

int iseq::getsteps()
{
    return _steps;
    
}

void iseq::setsubdiv(SUBDIV v)
{
    _subdiv = v;
    
}

SUBDIV iseq::getsubdiv()
{
    return _subdiv;
}

void iseq::setlooptype(SEQ_LOOP_TYPE loop)
{
    _loop = loop;
}

SEQ_LOOP_TYPE iseq::getlooptype()
{
    return _loop;
}



