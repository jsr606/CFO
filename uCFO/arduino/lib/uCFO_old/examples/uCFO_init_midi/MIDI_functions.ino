
void OnNoteOn(byte channel, byte note, byte velocity) {
  Midi.noteOn(channel, note, velocity);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  Midi.noteOff(channel, note, velocity);
}

void OnControlChange(byte channel, byte control, byte value) {
  Midi.controller(channel, control, value);  
}
