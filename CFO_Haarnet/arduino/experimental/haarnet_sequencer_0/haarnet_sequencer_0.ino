#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Haarnet.h>
#include <Sequencer.h>

// sequence ID
int s1;
// stepper index
int indx = 0;
const int nbr_notes = 16;
const int notes[] = {12, 24, 12, 12, 36, 12, 24, 12, 0, 12, 48, 36, 12, 24, 24, 36};


void setup() {
    
  Music.init();    
  
  Music.enableEnvelope1();
  Music.enableEnvelope2();

  
  // this is the sequencer code
  Sequencer.init();

  //Sequencer.newSequence(BPM, CALLBACK, SUBDIV);
  // create new sequence and ID (s1)
  s1 = Sequencer.newSequence(128, &s1cb, 4);    
  
  Serial.println("s index: " + s1);
  
  // start sequence 1
  Sequencer.startSequence(s1);
  
  //stop sequence 1
  // Sequencer.stopSequence(s1);
  
}

void loop() {
  Sequencer.update();
}


// callback function for the step sequencer
void s1cb() {
  Music.noteOn(notes[indx++] + 24, 127);
  if(indx > nbr_notes) indx = 0;
}

