//import oscP5.*;
//import netP5.*;
import controlP5.*;
import rwmidi.*;  // watch out for this
import processing.serial.*;

ControlP5 controlP5;
MidiInput input;
MidiOutput output;
Serial port0;

boolean printChange = true;
boolean serialBusy = false;

int backgroundColor = color(0,0,0);
int knobColor = color(235,103,295);

long time = 0;
long lastTime = 0;
long timePassed;
long timeAllowed = 10;

int delayTime = 40;

void setup() {
  
  size(400,400);
  smooth();
  background(0);
  
  setupKnobs();
    
  //println("print MIDI input devices:");
  println(RWMidi.getInputDeviceNames());
  input = RWMidi.getInputDevices()[0].createInput(this);
  println(RWMidi.getOutputDeviceNames());
  output = RWMidi.getOutputDevices()[4].createOutput();  
  //println(Serial.list());
//  port0 = new Serial(this, Serial.list()[12], 9600);
}


void draw() {
  background(backgroundColor);
  //writeKnobValues();
}


void keyPressed() {
  if(key=='s' || key=='S') {
    saveKnobValues();
  }
  if(key=='l' || key=='L') {
    loadKnobValues();
  }
}


void keyReleased() {
  if(key=='l' || key=='L') {
    sendMidiKnobs();
  }
}


