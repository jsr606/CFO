// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.

#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>

const int bodySwitch1 = A6;
const int bodySwitch2 = A3;
const int bodySwitch3 = A7;

long timeNow = 0;
long lastTime = 0;
long timeDelay = 500;

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  Music.setWaveform1(SAW);
  Music.setEnv1Sustain(127);
//  Music.setEnv2Sustain(127);
  Music.setCutoff(45*256);
  Music.setGain2(0.0);
  Music.setGain3(0.0);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);  
    
  // set internal pulldown resistors on bodyswitches
  *portConfigRegister(bodySwitch1) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  *portConfigRegister(bodySwitch2) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  *portConfigRegister(bodySwitch3) = PORT_PCR_MUX(1) | PORT_PCR_PE;
  

  
  
  
}

void loop() {

  usbMIDI.read();
  
  int a1 = analogRead(bodySwitch1);
  Serial.println(a1);
  int a2 = analogRead(bodySwitch2);
  int a3 = analogRead(bodySwitch3);
   
  timeNow = millis();
  if((timeNow - lastTime) > timeDelay) {
    
    int randomizer = random(5);
    int glide = 0;
    
    int oct = 1;
    if (randomizer == 0) {
      oct = map(a1,0,1023,0,4);
      glide = map(a2,0,1023,0,127);
    }
    Music.setPortamento(glide);
    Music.noteOn(24+oct*12);
    lastTime = timeNow;
  }
}
