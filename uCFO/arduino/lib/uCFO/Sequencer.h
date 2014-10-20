/*
 Sequencer.h - Friction Music library
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

#define MAX_SEQ 3

enum SUBDIV {
    NOTE_1 = 96,
    NOTE_2 = 48,
    NOTE_3 = 32,
    NOTE_4 = 24,
    NOTE_6 = 16,
    NOTE_8 = 12,
    NOTE_12 = 8,
    NOTE_16 = 6,
    NOTE_24 = 4,
    NOTE_32 = 3,
    NOTE_48 = 2,
    NOTE_96 = 1
};

typedef void (*func_cb)(void);

class seq;

class MSequencer {
public:
    void init(int bpm);
    void update();
    
    int newSequence(func_cb cb, SUBDIV subdiv);
    
    bool stopSequence(int index);
    bool startSequence(int index);

    bool setSequenceSubdiv(int index, SUBDIV subdiv);
    
    int getSequenceSubdiv(int index);
    
    bool setCallback(int index, func_cb cb);
    func_cb getCallback(int index);

    unsigned long clockStep;
    
    void setbpm(int v);
    int getbpm();



private:
    seq* _sequences[MAX_SEQ];
    int _bpm;
    int _bpmInClockSteps;

    
};

class seq {
    
    friend class MSequencer;
    
private:
    
    seq(int id, func_cb cb, SUBDIV subdiv);
    
    int _stepsize;
    SUBDIV _subdiv;
    int _tempo;
    int _id;
    
    unsigned long lastStep;
    unsigned long stepNum;
    unsigned long step;

    void setsubdiv(SUBDIV v);
    SUBDIV getsubdiv();
    
    bool _stopped;
    
    void callback(func_cb cb);
    
    func_cb _callback;
};


extern MSequencer Sequencer;
