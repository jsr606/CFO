/////////////
// BUTTONS //
/////////////
void readButtons() {
  // buttons are active low
  for(int i = 0; i < NUM_BUTTONS; i++) {
//    int i = buttonIndex++;
//    if(buttonIndex >= NUM_BUTTONS) buttonIndex = 0;
    buttonNow = millis();
    if((buttonNow - buttonTime[i]) > debounceTime) {
      buttonRead = digitalRead(buttonPin[i]);
      if(buttonRead != buttonState[i]) {
        buttonState[i] = buttonRead;
        buttonChange |= 1<<i;
        buttonTime[i] = buttonNow;
        machineState = 7 - buttonState[0] - buttonState[1] * 2 - buttonState[2] * 4;
      }
    }
  }
}


//////////
// KEYS //
//////////
void readKeys() {
  for (int i = 0; i < NUM_KEYS; i++) {
//    int i = keyIndex++;
//    if(keyIndex >= NUM_KEYS) keyIndex = 0;
    keyNow = millis();
    if((keyNow - keyTime[i]) > debounceTime) {
      keyRead = analogRead(keyPin[i]);
//      if(i==4) Serial.println(keyRead);
      if(keyRead > KEY_THRESHOLD) {
        keyValue = 1;
      } else {
        keyValue = 0;
      }
//      keyValue = (keyRead > KEY_THRESHOLD) ? 1 : 0 ;
      if(keyValue != keyState[i]) {
        keyState[i] = keyValue;
        keyChange |= 1<<i;
        keyTime[i] = keyNow;
      }
      if(keyState[i]) {
        keys |= 1<<i;
      } else {
        keys &= ~(1<<i);
      }
    }
  }
}


//////////
// POTS //
//////////
void checkBPM() {
  int bpm = analogRead(A0)>>2;
  if(bpm != _bpm) {
    _bpm = bpm;
    Serial.print("BPM set to ");
    Serial.println(_bpm);
    Sequencer.setbpm(_bpm);
    if(_bpm == 0) {
      Midi.setMidiIn(true);
      Midi.setMidiThru(true);
      Midi.setMidiOut(true);
      Midi.setMidiClockIn(true);
      Midi.setMidiClockThru(true);
      Midi.setMidiClockOut(true);
      Sequencer.setInternalClock(false);
    } else {
      Midi.setMidiIn(false);
      Midi.setMidiThru(false);
      Midi.setMidiOut(false);
      Midi.setMidiClockIn(false);
      Midi.setMidiClockThru(false);
      Midi.setMidiClockOut(false);
      Sequencer.setInternalClock(true);
//      Sequencer.sequencerContinue();
    }
  }
}



void initInterface() {
  pinMode(buttonPin[0], INPUT_PULLUP);
  pinMode(buttonPin[1], INPUT_PULLUP);
  pinMode(buttonPin[2], INPUT_PULLUP);
  pinMode(statusLed1,OUTPUT);
  for (int i = 0; i<8; i++) {
    pinMode(seqLed[i], OUTPUT);
  }
  startupAnimation();  
}


void updateLEDs() {
    switch(machineState) {
      case 0:
        leds = 0 | (1 << trackSelected);
        break;
      case 1:
        leds = 0 | (1 << noteSelected);
        leds = leds | (octave[stepSelected + 8 * trackSelected] << 7);
        break;
      case 2:
        leds = 0 | (1 << stepSelected);
        break;
      case 3: // nothing
        break;
      case 4:
        leds = 0 | (1 << trackSelected);
        break;
      case 5: // nothing
        break;
      case 6: // nothing
        break;
      case 7: // nothing
        break;
      default:
        break;
    }
  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[i], leds & (1 << i));
  }
}


void startupAnimation() {
  digitalWrite(statusLed1, HIGH);

  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[i],HIGH);
    delay(30); 
  }
  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[i],LOW);
    delay(30); 
  }

  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[7-i],HIGH);
    delay(30); 
  }
  for (int i = 0; i<8; i++) {
    digitalWrite(seqLed[7-i],LOW);
    delay(30); 
  }

  digitalWrite(statusLed1, LOW);
  
  delay(100);
}

