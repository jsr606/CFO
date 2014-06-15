/* 
 M3T3.h - Motors, Music and Motion library for Teensy 3.1
 Copyright (c) 2013 Science Friction. 
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
 + author: Jakob Bak
 + contact: j.bak@ciid.dk
 */

#ifndef CFO_BODYSEQ_h // include guard
#define CFO_BODYSEQ_h

#include <Arduino.h>
#include <spi4teensy3.h>
#include "BodyseqSynth.h"

///////////////////////////////////////////////////////////////////////
// THE BELOW FUNCTIONS ARE NEEDED FOR THE MUSIC PART TO RESPOND TO MIDI
///////////////////////////////////////////////////////////////////////

void OnNoteOn(byte channel, byte note, byte velocity) {
    Midi.noteOn(channel, note, velocity);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
    Midi.noteOff(channel, note, velocity);
}

void OnControlChange(byte channel, byte control, byte value) {
    Midi.controller(channel, control, value);
}

#endif // close guard CFO_BODYSEQ_h
