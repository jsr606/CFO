
#define MIDI_CHANNEL 1

#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>
#include <Mcp4251.h>
#include <HaarnetExtensionFlute.h>

void setup() {
  Music.init();

  Music.enableEnvelope1();
  Music.enableEnvelope2();

  // These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);

  // Loading a preset from EEPROM
  Music.getPreset(16);
  FluteEx.init();
  Serial.begin(115200);

  delay(1000);
  Music.dumpAllPresets();

}

void loop() {

}
