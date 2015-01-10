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
#define INSTR_SEQ 4
#define ISEQ_NBR_STEPS 32


#define TICKS_PER_QUARTER_NOTE 96 //

enum SUBDIV {
    NOTE_1 = (TICKS_PER_QUARTER_NOTE * 4),
    NOTE_1DOT = (TICKS_PER_QUARTER_NOTE * 3),
    NOTE_2 = (TICKS_PER_QUARTER_NOTE * 2),
    NOTE_3 = ((TICKS_PER_QUARTER_NOTE * 3) / 2),
    NOTE_4 = (TICKS_PER_QUARTER_NOTE * 1),
    NOTE_6 = ((TICKS_PER_QUARTER_NOTE * 2) / 3),
    NOTE_8 = (TICKS_PER_QUARTER_NOTE / 2),
    NOTE_12 = (TICKS_PER_QUARTER_NOTE / 3),
    NOTE_16 = (TICKS_PER_QUARTER_NOTE / 4),
    NOTE_24 = (TICKS_PER_QUARTER_NOTE / 6),
    NOTE_32 = (TICKS_PER_QUARTER_NOTE / 8),
    NOTE_48 = (TICKS_PER_QUARTER_NOTE / 12)
#if (TICKS_PER_QUARTER_NOTE > 64)
    ,NOTE_64 = (TICKS_PER_QUARTER_NOTE / 16),
    NOTE_96 = (TICKS_PER_QUARTER_NOTE / 24),
    NOTE_128 = (TICKS_PER_QUARTER_NOTE / 32),
    NOTE_192 = (TICKS_PER_QUARTER_NOTE / 48),
//    NOTE_256 = (TICKS_PER_QUARTER_NOTE / 64),
    NOTE_384 = (TICKS_PER_QUARTER_NOTE / 92)
#endif
};

enum SEQ_LOOP_TYPE {
    NO_LOOP = 0,
    FORWARD_LOOP = 1,
    BACKWARD_LOOP = 2,
    PINGPONG = 3
};

typedef void (*func_cb)(void);

class seq;
class iseq;

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


class iSequencer {
public:
    void init(int bpm);
    void update();
    
    int newSequence(SUBDIV subdiv, int steps, SEQ_LOOP_TYPE loop);
    
    bool stopSequence(int index);
    bool startSequence(int index);
    
    bool setSequenceSubdiv(int index, SUBDIV subdiv);
    
    int getSequenceSubdiv(int index);
    
//    bool setCallback(int index, func_cb cb);
//    func_cb getCallback(int index);
    
    unsigned long clockStep;
    
    void setbpm(int v);
    int getbpm();
    
    
    
private:
    iseq* _sequences[INSTR_SEQ];
    int _bpm;
    int _bpmInClockSteps;
    
    
};


class seq {
    
    friend class MSequencer;
    
private:
    
    seq(int id, func_cb cb, SUBDIV subdiv);
    
    int _id;
    SUBDIV _subdiv;
    
    unsigned long lastStep;
    unsigned long stepNum;
    unsigned long step;

    void setsubdiv(SUBDIV v);
    SUBDIV getsubdiv();
    
    bool _stopped;
    
    void callback(func_cb cb);
    
    func_cb _callback;
};


class iseq {
    
    friend class iSequencer;
    
private:
    
    iseq(int id, SUBDIV subdiv, int steps, SEQ_LOOP_TYPE loop);
    
    int _id;
    int _steps;
    int _length;
    int _direction;
    SUBDIV _subdiv;
    SEQ_LOOP_TYPE _loop;
    
    int indx;

    int _notes[];
    int _velocity[];
    int _ccNumbers[];
    int _ccValues[];
    
    unsigned long lastStep;
    unsigned long stepNum;
    unsigned long step;
    
    void setsubdiv(SUBDIV v);
    SUBDIV getsubdiv();

    void setsteps(int steps);
    int getsteps();

    void setlooptype(SEQ_LOOP_TYPE loop);
    SEQ_LOOP_TYPE getlooptype();
    

    
    bool _stopped;

};


extern MSequencer Sequencer;
extern iSequencer iSeq;
