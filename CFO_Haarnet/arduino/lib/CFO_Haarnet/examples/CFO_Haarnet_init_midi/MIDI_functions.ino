
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
