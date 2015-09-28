#define MIDI_CHANNEL 1

#include <spi4teensy3.h>
#include <EEPROM.h>
#include <CFO_BODYSEQ.h>

// sequence ID
int s1;

// sequence step index
int indx1 = 0;

const int nbr_notes1 = 16;

int BD[] = {1,0,0,1,  0,0,1,0,  0,0,1,0,  0,0,0,0};
int SN[] = {0,0,0,0,  1,0,0,0,  0,0,0,0,  1,0,0,0};
int RS[] = {0,0,1,0,  0,1,0,0,  0,0,1,0,  0,1,0,0};
int CP[] = {0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,1};
int HH[] = {1,0,1,0,  1,1,0,1,  0,1,0,1,  0,1,0,1};
int OH[] = {0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,1,0};
int CL[] = {0,1,0,0,  0,0,1,0,  0,0,0,1,  0,1,1,0};
int CB[] = {0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};


//int BD[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};
//int SN[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};
//int RS[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};
//int CP[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};
//int HH[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};
//int OH[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};
//int CL[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};
//int CB[] = {1,1,1,1,  1,1,1,1,  1,1,1,1,  1,1,1,1};

void setup() {

  // We initialise the sound engine by calling Music.init() which outputs a tone
  Music.init();
  Music.setSampler(true);
  Music.setSynth(false);
  Music.enableEnvelope1();
  Music.enableEnvelope2();
    
// These guys just have to be here...
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
  
// this is the sequencer code
  Sequencer.init(120);
  Sequencer.setInternalClock(true);  

  //Sequencer.newSequence(SUBDIV, CALLBACK);
  s1 = Sequencer.newSequence(NOTE_16, &s1cb);
//  Sequencer.setInternal(s1, true);
//  Sequencer.setExternal(s1, false);

  // start sequences
  Sequencer.startSequence(s1);
}

void loop() {
  
// Checking for incoming MIDI to use dashboard
  usbMIDI.read();
  Sequencer.update();
  Midi.checkSerialMidi();
}

// callback function for the step sequencer

void s1cb() {
  if(BD[indx1]) Music.noteOnSample(0);
  if(SN[indx1]) Music.noteOnSample(1);
  if(RS[indx1]) Music.noteOnSample(2);
  if(CP[indx1]) Music.noteOnSample(3);
  if(HH[indx1]) Music.noteOnSample(4);
  if(OH[indx1]) Music.noteOnSample(5);
  if(CL[indx1]) Music.noteOnSample(6);
  if(CB[indx1]) Music.noteOnSample(7);

  indx1++;
  if(indx1 >= nbr_notes1) indx1 = 0;
}
