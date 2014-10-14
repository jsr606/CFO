/* 
 Haarnet.h - Friction Music library
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

#ifndef Haarnet_h // include guard
#define Haarnet_h

#include <Arduino.h>
#include <spi4teensy3.h>
#include "Synth.h"
#include "Sequencer.h"

///////////////////////////////////////////////////////////////////////
// THE BELOW FUNCTIONS ARE NEEDED FOR THE MUSIC PART TO RESPOND TO MIDI
///////////////////////////////////////////////////////////////////////

void OnNoteOn(byte channel, byte note, byte velocity) {
    if(channel == MIDI_CHANNEL) {
        Midi.noteOn(channel, note, velocity);
    }
    channel = channel - 1;
    MIDI_SERIAL.write(byte(0x90 | (channel & 0x0F)));
    MIDI_SERIAL.write(0x7F & note);
    MIDI_SERIAL.write(0x7F & velocity);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
    if(channel == MIDI_CHANNEL) {
        Midi.noteOff(channel, note, velocity);
    }
    channel = channel - 1;
    MIDI_SERIAL.write(byte(0x80 | (channel & 0x0F)));
    MIDI_SERIAL.write(0x7F & note);
    MIDI_SERIAL.write(0x7F & velocity);
}

void OnControlChange(byte channel, byte control, byte value) {
    if(channel == MIDI_CHANNEL) {
        Midi.controller(channel, control, value);
    }
    channel = channel - 1;
    MIDI_SERIAL.write(byte(0xB0 | (channel & 0x0F)));
    MIDI_SERIAL.write(0x7F & control);
    MIDI_SERIAL.write(0x7F & value);
}


//usbMIDI.setHandleNoteOff(OnNoteOff);
//usbMIDI.setHandleNoteOn(OnNoteOn);
//usbMIDI.setHandleControlChange(OnControlChange);



#endif // close guard Haarnet_h
