void noteOnReceived(Note note) {
  
  byte channel = byte(note.getChannel());
  byte pitch = byte(note.getPitch());
  byte velocity = byte(note.getVelocity());

  sendNoteOn(channel, pitch, velocity);
  
}

void noteOffReceived(Note note) {

  byte channel = byte(note.getChannel());
  byte pitch = byte(note.getPitch());
  byte velocity = byte(note.getVelocity());
  
  sendNoteOff(channel, pitch, velocity);
  
}

void controllerChangeReceived(rwmidi.Controller controller) {
  
  byte channel = byte(controller.getChannel());
  byte CC = byte(controller.getCC());
  byte value = byte(controller.getValue());
  
  sendControlChange(channel, CC, value);
}


void sendNoteOn(byte channel, byte pitch, byte velocity) {

  byte noteOn = byte(0x90 | channel);

  serialBusy = true;
  output.sendNoteOn(channel, pitch, velocity);
//  port0.write(noteOn);
//  port0.write(pitch);
//  port0.write(velocity);
//  //println('\n' + hex(noteOn) + " " + hex(pitch) + " " + hex(velocity));
  serialBusy = false;
}

void sendNoteOff(byte channel, byte pitch, byte velocity) {

  byte noteOff = byte(0x80 | channel);  

  serialBusy = true;
  output.sendNoteOff(channel, pitch, velocity);
//  port0.write(noteOff);
//  port0.write(pitch);
//  port0.write(velocity);
//  //println('\n' + hex(noteOff) + " " + hex(pitch) + " " + hex(velocity));
  serialBusy = false;
}

void sendControlChange(byte channel, byte CC, byte value) {
  time = millis();
  timePassed = time - lastTime;
  if(timePassed > timeAllowed) {
    lastTime = time;
    
    byte controlChange = byte(0xB0 | channel);
    
    serialBusy = true;
    output.sendController(channel, CC, value);
//    port0.write(controlChange);
//    port0.write(CC);
//    port0.write(value);
    //println('\n' + hex(controlChange) + " " + hex(CC) + " " + hex(value));
    serialBusy = false;
  } 
}
