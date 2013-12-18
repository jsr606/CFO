// The Music object is automatically instantiated when the header file is
// included. Make calls to the Music objects with "Music.function(args)".
// You still need to call Music.init() in the setup() function below.
#define MIDI
#define MIDI_CHANNEL 1

#include <SPI.h>
#include <Friction.h>
#include <MotorT3.h>

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Serial.begin(115200);
  MotorA.init();
  MotorB.init();
  Music.init();
  Music.enableEnvelope1();
  Music.enableEnvelope2();
//  Midi.init();
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  
  analogReadAveraging(32);
}

void loop() {

  usbMIDI.read();
  int valueA = analogRead(1);
  int valueB = analogRead(8);
//  Serial.print("valueA = ");
//  Serial.print(valueA);
//  Serial.print(" --- valueB = ");
//  Serial.println(valueB);
//  delay(50);
  MotorA.torque((512-valueA)*4);
  MotorB.torque((512-valueB)*4);
  
}
