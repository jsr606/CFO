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

#pragma once

#define MAX_SEQ 3

typedef void (*func_cb)(void);

class seq;

class MSequencer {
public:
    void init();
    void update();
    
    int newSequence(int bpm, func_cb cb);
    
    bool stopSequence(int index);
    bool startSequence(int index);

private:
    seq* _sequences[MAX_SEQ];
    
};

class seq {
    
    friend class MSequencer;
    
private:
    
    seq(int bpm, func_cb cb);
    
    int _bpm;
    int _tempo;
    
    unsigned long ltick;
    
    void setbpm(int v);
    int getbpm();
    
    bool _stoped;
    
    void callback(func_cb cb);
    
    func_cb _callback;
};


extern MSequencer Sequencer;
