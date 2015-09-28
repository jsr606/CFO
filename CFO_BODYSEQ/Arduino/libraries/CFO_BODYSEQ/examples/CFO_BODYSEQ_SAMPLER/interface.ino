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
  int keyr = 0;
  int keyv = 0;
  for (int i = 0; i < NUM_KEYS; i++) {
    keyNow = millis();
    if((keyNow - keyTime[i]) > debounceTime) {
      keyRead = analogRead(keyPin[i]);
      if(keyRead > KEY_THRESHOLD) {
        keyValue = 1;
      } else {
        keyValue = 0;
      }
      if(i == 0) {
        keyr = keyRead;
        keyv = keyValue;
      }
//      keyValue = (keyRead > KEY_THRESHOLD) ? 1 : 0 ;
      if(keyState[i] != keyValue) {
        keyState[i] = keyValue;
        keyChange |= 1<<i;
        keyTime[i] = keyNow;
      }
    }
  }
  for (int i = 0; i < NUM_KEYS; i++) {
      if(keyState[i] == 1) {
        keys |= (1<<i);
      } else if(keyState[i] == 0) {
        keys &= ~(1<<i);
      }
  }
  Serial.printf("keyTime[0]=%10ld, keyr=%i, keyv=%i - keyState is %i%i%i%i%i%i%i%i, keys is %X \n", keyTime[0], keyr, keyv, keyState[0], keyState[1], keyState[2], keyState[3], keyState[4], keyState[5], keyState[6], keyState[7], keys);
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
  int t = trackSelected;
  int s = sampleSelected;
  switch(machineState) {
    case 0:
      leds = 0 | (1 << trackPlaying);
      break;
    case 1:
      leds = 0;
      for(int i=0; i<NUM_STEPS; i++) {
        leds = leds | (sample[t][s][i] << i);
      }
      break;
    case 2:
      leds = 0 | (1 << sampleSelected);
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

