/*
==============================================================================
 ::::::::  :::::::::: ::::::::  
:+:    :+: :+:       :+:    :+: 
+:+        +:+       +:+    +:+ 
+#+        :#::+::#  +#+    +:+ 
+#+        +#+       +#+    +#+ 
#+#    #+# #+#       #+#    #+# 
 ########  ###        ########  
::::::::::: :::::::::      :::      ::::::::  :::    ::: :::::::::: :::::::::  
    :+:     :+:    :+:   :+: :+:   :+:    :+: :+:   :+:  :+:        :+:    :+: 
    +:+     +:+    +:+  +:+   +:+  +:+        +:+  +:+   +:+        +:+    +:+ 
    +#+     +#++:++#:  +#++:++#++: +#+        +#++:++    +#++:++#   +#++:++#:  
    +#+     +#+    +#+ +#+     +#+ +#+        +#+  +#+   +#+        +#+    +#+ 
    #+#     #+#    #+# #+#     #+# #+#    #+# #+#   #+#  #+#        #+#    #+# 
    ###     ###    ### ###     ###  ########  ###    ### ########## ###    ### 
==============================================================================
simple tracker / instrument editor for CHEAP, FAT and OPEN synthesizer
more info: http://8bitklubben.dk/project/CFO/
==============================================================================
*/

import controlP5.*;
import processing.serial.*;
import java.awt.FileDialog;
import rwmidi.*;

//setup MIDI
int midiInput = 1;                   // on my mac this is normally IAC Driver - IAC Bus 2 Apple Inc.
int midiOutput = 0;                  // on my mac this is normally IAC Driver - Bus 1 Apple Inc.

MidiInput input;
MidiOutput output;

//for clipboard helper
import java.awt.datatransfer.*;
import java.awt.Toolkit; 
ClipHelper cp = new ClipHelper();

ControlP5 controlP5;

PFont font, tinyFont;
String trackFile[];
ArrayList trackData, noteData, commandData;
String settings[];
int textSpacing = 16;
int xOffset = 30;
int yOffset = 35;
Serial serialPort;

int xPos[] = {0,1,4,5,7,10,11,12,13};
int ID_POS = 0;
int NOTE_POS = 2;
int COMMAND_POS = 5;

int cursX=0;
int cursY=0;
int cursW=10;
int cursH=16;

int step = -1;
int steps;
int subSteps = 4;
int bpmTracker = 120;
int bpmSynth = 120;
float millisPerBeat;
float lastBeat;
boolean running = false;

String noteNames[] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "H "};

//instrument
int family = 0;
int ID = 0;
int waveform = 0;
int gain = 255;
int sustain = -1;
int decay = 0;

int activeID;
int killTime;

boolean monitor = false;

int nStart,nLength,nSubsteps,nLoops;
int cStart,cLength,cSubsteps,cLoops;

//computer optimized (bytes, ascii values)
byte[] byteTransmit = new byte[5];

PFont smallFont;
String[] instructions;

boolean ignoreID = true;

void setup() {
  size(600,600);  
  frameRate(50);

  //setup midi
  listMidiDevices();
  input = RWMidi.getInputDevices()[midiInput].createInput(this);    //midi input
  output = RWMidi.getOutputDevices()[midiOutput].createOutput();    //midi output 

  calculateDelays();

  //controlP5
  controlP5 = new ControlP5(this);
  controlP5.setColorActive(color(0,100,100,255));
  controlP5.setColorBackground(0x20ffffff);
  controlP5.setColorForeground(0xb0aaaaaa);
  controlP5.setColorLabel(0xffffffff);
  controlP5.setColorValue(0xffffffff);

  //file operations
  int p5x = 196, p5y = 32;
  int buttWidth = 82, buttHeigth = 10, labelWidth = 120, spacing = 11, id=0, labelXOffset = -5, labelYOffset = 2;
  controlP5.addTextlabel("file_io_label","FILE IO",p5x+labelXOffset,p5y+labelYOffset).setWidth(labelWidth);
  p5y += spacing;
  controlP5.addButton("LOAD",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("SAVE",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("COPY SEQUENCE",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("////////////",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("MONITOR",1,p5x,p5y,buttWidth,buttHeigth).setId(id);

  //instrument editor
  p5y += spacing*2;
  id++;
  controlP5.addTextlabel("editor_label","INSTRUMENT EDITOR",p5x+labelXOffset,p5y+labelYOffset).setWidth(labelWidth);
  p5y += spacing; 
  id++;
  controlP5.addSlider("family",0,127,0,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("ID",0,127,0,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("waveform",0,15,0,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("gain",0,127,255,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("sustain",-1,126,-1,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("decay",-64,63,0,p5x,p5y,255,10).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("SEND INSTRUMENT",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("RESET",1,p5x,p5y,buttWidth,buttHeigth).setId(id);

  //loop editor
  p5y += spacing*2;
  id++;
  controlP5.addTextlabel("editor_label","LOOP EDITOR",p5x+labelXOffset,p5y+labelYOffset).setWidth(labelWidth);
  p5y += spacing; 
  id++;
  controlP5.addSlider("nStart",0,255,0,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("nLength",1,255,44,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("nSubsteps",1,32,4,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("nLoops",-1,255,-1,p5x,p5y,255,10).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("SEND LOOPS",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("cStart",0,255,0,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("cLength",1,255,32,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("cSubsteps",1,32,4,p5x,p5y,255,10).setId(id);
  p5y += spacing; 
  id++;
  controlP5.addSlider("cLoops",-1,254,-1,p5x,p5y,255,10).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("SEND COMMANDS",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("SEND BOTH",1,p5x,p5y,buttWidth,buttHeigth).setId(id);

  //tempo
  p5y += spacing*2;
  id++;
  controlP5.addTextlabel("editor_label","TEMPO SYNTH",p5x+labelXOffset,p5y+labelYOffset).setWidth(labelWidth);
  p5y += spacing; 
  id++;
  controlP5.addSlider("bpmSynth",60,255,120,p5x,p5y,255,10).setId(id);  
  println("id:"+id);  
  p5y += spacing;
  id++;
  controlP5.addButton("START SYNTH",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("STOP SYNTH",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing*2;
  id++;
  controlP5.addTextlabel("editor_label","TEMPO TRACKER",p5x+labelXOffset,p5y+labelYOffset).setWidth(labelWidth);
  p5y += spacing; 
  id++;
  controlP5.addSlider("bpmTracker",1,500,120,p5x,p5y,255,10).setId(id);  
  println("id:"+id);  
  p5y += spacing;
  id++;
  controlP5.addButton("START TRACKER",1,p5x,p5y,buttWidth,buttHeigth).setId(id);
  p5y += spacing;
  id++;
  controlP5.addButton("STOP TRACKER",1,p5x,p5y,buttWidth,buttHeigth).setId(id);

  //trackData keeps music data
  trackData = new ArrayList();
  noteData = new ArrayList();
  commandData = new ArrayList();
  //trackFile is used for file operations
  trackFile = loadStrings("data/track01.txt");
  steps = trackFile.length;

  //copy to trackData
  for (int i=0;i<steps;i++) {
    trackData.add(trackFile[i]);
  }

  //font stuff
  font = loadFont("PrestigeEliteStd-Bd-16.vlw");
  tinyFont = loadFont("Skia-Regular-10.vlw");
  textFont(font);

  //println(Serial.list());  // prints all available serial ports
  serialPort = new Serial(this, Serial.list()[0], 2400);

  //bpm
  println("bpm: "+bpmTracker);
  println("with "+subSteps+" subSteps, this equals "+millisPerBeat+" millis per substep");
  lastBeat = millis()-millisPerBeat; //so we start straight away
  
  smallFont = loadFont("TIME2-8.vlw");
  instructions = loadStrings("instructions.txt");
}

void draw () {
  background(0);

  //check for beat
  if (running && millis()-lastBeat > millisPerBeat) {
    lastBeat = millis();
    tick();
  } 

  drawTracker();

  while (serialPort.available() > 0) {
    int inByte = serialPort.read();
    print(char(inByte));
  }

  textFont(smallFont);
  fill(255);
  for (int i = 0; i<instructions.length; i++) {
    text(instructions[i],330,15+i*6);
  }
  
  if (ignoreID) {
    fill(0,200);
    rect(30,0,26,height);
  }
}  

void calculateDelays() {
  millisPerBeat = 60000/bpmTracker/subSteps; //how many millis pr beat
}

void tick() {
  step++;
  step = step % steps;

  //parse our text from tracker interface to bytes
  String stepData = (String) trackData.get(step);

  if (stepData.equals("               ")) {
    return; // nevermind, line is empty
  } 
  else {
    //ID
    parseID(stepData);
    //note
    parseNotes(stepData);
    //command
    parseCommand(stepData);
  }
  //transmit collected bytes over serial port
  serialPort.write(128+2); //STX

  for (int i = 0; i<byteTransmit.length; i++) {
    serialPort.write(byteTransmit[i]);
    if (monitor) print(int(byteTransmit[i])+"\t");
  }
  if (monitor) println();
}

void compileNotes() {
  String notes = "";
  notes += (""+steps)+char(9);
  for (int i = 0; i<steps; i++) {
    String stepData = (String) trackData.get(i);
    parseNotes(stepData);

    notes += int(byteTransmit[1])+","+int(byteTransmit[2]);
    if (i != steps-1) notes += ",";
  }
  //println();
  println(notes);
  cp.copyString(notes+char(10));
}

void compileSequence() {
  String notes = "";
  //notes += (""+steps)+char(9);
  for (int i = 0; i<steps; i++) {
    String stepData = (String) trackData.get(i);
    parseNotes(stepData);
    notes += int(byteTransmit[1])+","+int(byteTransmit[2]);
    parseCommand(stepData);    
    notes += ","+int(byteTransmit[3])+","+int(byteTransmit[4])+",";
    //if (i != steps-1) notes += ",";
  }
  //println();
  println(notes);
  cp.copyString(notes+char(10));
}

void compileCommands() {
  String commands = "";
  //commands += (""+steps+char(9));

  for (int i = 0; i<steps; i++) {
    String stepData = (String) trackData.get(i);
    parseCommand(stepData);

    commands += int(byteTransmit[3])+","+int(byteTransmit[4]);

    if (i != steps-1) commands += ",";
  }
  println(commands);
  cp.copyString(commands+char(10));
}

void parseID(String data) {
  //parse ID
  if (ignoreID) {
    byteTransmit[0] = byte(128+42); //'*'
    return;
  }
    
  char ID1 = data.charAt(xPos[ID_POS]);
  char ID2 = data.charAt(xPos[ID_POS+1]);
  if (ID2 == ' ') { //empty
    byteTransmit[0] = byte(activeID); //255 is the new 32
  } 
  else if (ID2 == '*') {
    byteTransmit[0] = byte(128+42); //'*'
  } 
  else {
    //if (ID1 == ' ') ID1 = '0';
    int ID = getInt(ID1,ID2);
    if (ID == -1) {
      byteTransmit[0] = byte(255);
      //print("unknown ID");
    } 
    else {
      byteTransmit[0] = byte(ID);
    }
  }
}

void parseNotes(String data) {
  //note
  if (data.charAt(xPos[NOTE_POS]) == ' ') {
    //no notes here
    byteTransmit[1] = byte(255);
    byteTransmit[2] = byte(255);
  } 
  else {
    //parse note value + octave
    //make note name from two chars
    char note1 = data.charAt(xPos[NOTE_POS]);
    char note2 = data.charAt(xPos[NOTE_POS+1]);
    char noteCollect[] = {
      note1,note2
    };
    String note = new String (noteCollect);
    //compare to noteNames
    int noteVal = -1;
    for (int i = 0; i<noteNames.length; i++) {
      if (note.equals(noteNames[i])) {
        noteVal = i;
      }
    }

    if (noteVal != -1) {
      byteTransmit[1] = byte(noteVal);

      //add octave
      char oct = data.charAt(xPos[NOTE_POS+2]);
      byte octVal = byte(oct-48);
      if (octVal >= 0 && octVal <10) {
        byteTransmit[2] = byte(octVal);
      }
      else {
        //octave field empty
        byteTransmit[2] = byte(255);
      }
    }
  }
}

void parseCommand(String data) {
  //command
  if (data.charAt(xPos[COMMAND_POS]) == ' ') {
    //no commands here
    byteTransmit[3] = byte(255);
    byteTransmit[4] = byte(255);
  } 
  else {
    //parse command + value
    char command1 = data.charAt(xPos[COMMAND_POS]);
    char command2 = data.charAt(xPos[COMMAND_POS+1]);
    char value1 = data.charAt(xPos[COMMAND_POS+2]);
    char value2 = data.charAt(xPos[COMMAND_POS+3]);
    char value3 = data.charAt(xPos[COMMAND_POS+1]);

    switch (command1) {
    case 'K':
      //KILL
      byteTransmit[3] = byte(128+75); //'K'
      int killTime = getInt3(value3,value1,value2); //killTime defines millis() delay to kill command
      byteTransmit[4] = byte(killTime);
      break;

    case 'W':
      if (command2 == ' ' || command2 == 'A') {
        //absolute
        byteTransmit[3] = byte(128+87); //'W'
        int value = getInt (value1,value2);
        byteTransmit[4] = byte(value);
      }
      break;

    case 'B':
      byteTransmit[3] = byte(128+66); //'B'
      int bpmValue = getInt3(value3,value1,value2);
      //print("BPM: "+bpmValue);
      byteTransmit[4] = byte(bpmValue);
      bpmTracker = int(map(bpmValue,1,255,1,1000));
      calculateDelays();
      break;
      
    case 'O':
      //OCTAVE
      if (command2 == ' ' || command2 == 'C') {
        //absolute
        byteTransmit[3] = byte(128+79); //'O'
        int value = getInt (value1,value2);
        byteTransmit[4] = byte(value);
        break;
      }
      if (command2 == '+') {
        //add
        byteTransmit[3] = byte(128+111); //'o'
        int value = getInt (value1,value2);
        //if (value == -1) break; //TODO: fix return value to be -1000 if error!
        //println("oct, rel+, byte val: "+(64+value));
        byteTransmit[4] = byte(64+value);
      }
      if (command2 == '-') {
        //subtract
        byteTransmit[3] = byte(128+111); //'o'
        int value = getInt (value1,value2);
        //println("oct, rel-, byte val: "+byte(64-value));
        byteTransmit[4] = byte(64-value);
      }
      break;

    case 'G':
      //GAIN
      if (command2 == '+') {
        //add
        byteTransmit[3] = byte(128+103); //'g'
        if (value1 == ' ') value1 ='0';
        int value = getInt (value1,value2);
        //println("gain+: "+value);
        byteTransmit[4] = byte(64+value);
        break;
      }
      if (command2 == '-') {
        //subtract
        byteTransmit[3] = byte(128+103); //'g'
        if (value1 == ' ') value1 ='0';
        int value = getInt (value1,value2);
        //println("gain-: "+value);
        byteTransmit[4] = byte(64-value); 
        break;
      }
      //if not relative, absolute:
      byteTransmit[3] = byte(128+71); //'G'
      if (value1 == ' ') value1 ='0';
      if (value3 == ' ') value3 ='0';
      int value = getInt3 (value3,value1,value2);
      //println("gain: "+value);
      byteTransmit[4] = byte(value); 
      break;
    }
  }
}

int getInt (char c1, char c2) {
  if (c1 == 32 && c2 == 32) return -1; 
  if (c1 == ' ') c1 = '0';
  char cCollect[] = {
    c1,c2
  };
  String cString = new String (cCollect);
  int returnVal;
  try {
    returnVal = Integer.parseInt(cString);
  } 
  catch (NumberFormatException e) {
    returnVal = -1;
  }
  return returnVal;
}

int getInt3 (char c1, char c2, char c3) {   
  if (c1 == 32 && c2 == 32) return -1; 
  if (c1 == 32) c1 = '0';
  if (c2 == 32) c2 = '0';
  char cCollect[] = {
    c1,c2,c3
  };
  String cString = new String (cCollect);
  int returnVal;
  try {
    returnVal = Integer.parseInt(cString);
  } 
  catch (NumberFormatException e) {
    returnVal = -1;
  }
  return returnVal;
}


void keyPressed() {
  //navigation
  if (key == CODED) {
    if (keyCode == RIGHT) {
      cursX = (cursX+1) % xPos.length; 
      return;
    }
    if (keyCode == LEFT) {
      cursX --;
      if (cursX == -1) cursX = xPos.length-1;
      return;
    }
    if (keyCode == UP) {
      cursY --;
      if (cursY == -1) cursY = steps-1; 
      return;
    }
    if (keyCode == DOWN) {
      cursY = (cursY+1) % steps; 
      return;
    }
    return;
  }

  //track length
  if (key == 'T') {
    steps ++;
    steps = constrain(steps,1,32);
    if (trackData.size()<32) {
      trackData.add(trackData.size(),"               ");
    }
    return;
  }
  if (key == 't') {
    steps --;
    steps = constrain(steps,1,32);
    cursY = min(cursY,steps-1);
    return;
  }
  
  if (key == 'i' || key == 'I') {
    ignoreID = !ignoreID;
    return;
  }

  //not allowed
  if (key == DELETE || key == BACKSPACE || key == ESC || key == ENTER || key == RETURN || key == TAB) {
    println("tut tut");
    return;
  }

  //start/stop
  if (key == 'r' || key == 'R') {
    running = !running;
    return;
  }

  //everything else goes into the track at current cursor position
  String newString="";
  String currentString = (String) trackData.get(cursY);
  for (int i = 0; i<currentString.length() ; i++ ) {
    if (i == xPos[cursX]) {
      String goUpperCase = ""+key;
      newString += goUpperCase.toUpperCase();
    } 
    else {
      newString += currentString.charAt(i);
    }
  }

  //println(newString);
  trackData.set(cursY,newString);
}


void mousePressed() {
  if (monitor) println(mouseX+" "+mouseY);
}

void drawTracker() {
  //label columns, draw tracker
  for (int i = 0; i<steps; i++) {
    if (i%subSteps == 0) {
      fill(50,200);
    } 
    else {
      fill(25,200);
    }
    rect (xOffset-2,yOffset+cursH*i+2,152,-cursH);
  }

  //update progress bar
  float f = constrain(255-(millis()-lastBeat)/millisPerBeat*150,50,200);
  fill(f);
  rect (xOffset-2,yOffset+cursH*step+2,152,-cursH);

  stroke(150);
  line(22,0,22,height);
  line(61,0,61,height);
  line(120,0,120,height);
  fill(100);
  textFont(font);
  text("ID  note  cmnd  bpm "+bpmTracker,xOffset,textSpacing);

  //draw cursor
  noStroke();
  fill(0,100,100);
  rect(xOffset+cursW*(xPos[cursX])-1,yOffset+cursY*cursH+2,cursW,cursH*-1);

  //tracker update
  int textY = yOffset;
  for (int i=0; i<steps; i++) {
    fill(100);
    textFont(tinyFont);
    text(i,5,textY);
    fill(255);
    textFont(font);
    String data = (String) trackData.get(i);
    text(data,xOffset,textY);
    textY += textSpacing;
  }
}

void stop() {
  println("bye");
  serialPort.write(byte(128+2));
  serialPort.write(byte(128+42));
  serialPort.write(byte(255));
  serialPort.write(byte(255));
  serialPort.write(byte(128+75)); //KILL command
  serialPort.write(byte(255));
} 


void controllerChangeReceived(rwmidi.Controller cc) {

  int theController = cc.getCC();
  float theControllerValue = cc.getValue();
  //println("got CC: "+theController+" with value: "+theControllerValue);

  if (theController >=14 && theController < 20) {
    int led = theController - 14;
    int ledBrightness = int(map(theControllerValue, 0, 127, 0, 255));

  }
}

void noteOnReceived(Note note) {
  int noteVal = note.getPitch();
  int velocity = note.getVelocity();
  println("note on " + note + " with velocity " + velocity);
  
  byte byteTransmit[] = {0,0,0,0,0};
  if (noteVal >= 48) {
    
    byteTransmit[0] = byte(128+42); //'*' = all synth IDs
    
    print("got midi val: "+noteVal);
    int sendNote = (noteVal-48)%12;
    print("\tthis translates to note "+sendNote);
    byteTransmit[1] = byte(sendNote);
    int oct = int((noteVal-48)/12);
    println(" in octave "+oct);
    byteTransmit[2] = byte(oct);
    byteTransmit[3] = byte(128+103); //'g' == gain
    byteTransmit[4] = byte(velocity);
    
    //transmit data over serial port
    serialPort.write(128+2); //STX
    print("sending midi: \t");
    for (int i = 0; i<byteTransmit.length; i++) {
      serialPort.write(byteTransmit[i]);
      print(int(byteTransmit[i])+"\t");
    }
    println();
  }
}

void noteOffReceived(Note note) {
  int noteVal = note.getPitch();
  //println("note off " + note.getPitch());
  /*
  if (noteVal >= 48 && noteVal < 54) {
   int pin = noteVal - 48;
   arduino.analogWrite(ledPins[pin], 0);
   pwmVal[pin] = 0;
   //println("pin: "+ledPins[pin]+" - 0");
   }
   */
}

void listMidiDevices() {
  //list midi inputs
  String [] names = RWMidi.getInputDeviceNames();
  println();
  println("midi input devices available:");
  //int midiControl = -1;
  //String midiController = "IAC Driver - Bus 1 Apple Inc.";
  for (int i = 0; i<names.length; i++) {
    println(i+": "+names[i]);
    //if (names[i].equals(midiController)) midiControl = i;
  }

  // if (midiControl != -1) {
  //   input = RWMidi.getInputDevices()[midiControl].createInput(this);
  // }

  MidiOutputDevice devices[] = RWMidi.getOutputDevices();
  //list all output devices
  println();
  println("midi output devices available:");
  for (int i = 0; i < devices.length; i++) {
    println(i + ": " + devices[i].getName());
  }
}


void controlEvent(ControlEvent theEvent) 
{
  switch(theEvent.controller().id()) {

    //load pattern
    //TODO: bpm and pattern length is not updated
    case(0):
    String loadPath = selectInput("load bleeps, yo");  // Opens file chooser
    if (loadPath == null) {
      println("no file was selected...");
    }   
    else {
      println("loadingFrom: "+loadPath);
      trackFile = loadStrings(loadPath);
      trackData.clear();
      for (int i=0; i<trackFile.length; i++) {
        trackData.add(trackFile[i]);
      }
    }
    break;

    //save pattern
    case(1): 
    String savePath = selectOutput();  // Opens file chooser
    trackFile = new String[steps];
    for (int i = 0;i<steps;i++) {
      trackFile[i] = (String) trackData.get(i);
    }

    if (savePath == null) {
      println("no file was selected...");
    } 
    else {
      println("saving to: "+savePath+".txt");
      saveStrings(savePath+".txt",trackFile);
    }
    break;

    case(2):
    println("COPY SEQUENCE");
    //compileNotes();      
    compileSequence();
    break;

    case(3):
    println("don't clickkkk me!");
    //compileCommands();      
    break;

    case(4):
    println("MONITOR");
    monitor = !monitor;
    break;

    //upload instrument
    case(12):
    println("UPLOAD");
    serialPort.write(128+3); //STX2
    serialPort.write(waveform);
    serialPort.write(gain);
    serialPort.write(sustain);
    serialPort.write(decay);
    break;

    //reset
    case(13):
    controlP5.controller("ID").setValue(0);
    controlP5.controller("waveform").setValue(0);
    controlP5.controller("gain").setValue(127);
    controlP5.controller("sustain").setValue(-1);
    controlP5.controller("decay").setValue(0);
    break;

    //send notes
    case(19):
    println("SEND NOTES");
    serialPort.write(128+4); //STX sequencer commands
    serialPort.write(byte(255));
    serialPort.write(byte(255));
    serialPort.write(byte(255));
    serialPort.write(byte(255));
    serialPort.write(nStart);
    serialPort.write(nLength);
    serialPort.write(nSubsteps);
    serialPort.write(nLoops);
    break;

    //send commands
    case(24):
    println("SEND COMMANDS"); 
    serialPort.write(128+4); //STX sequencer commands
    serialPort.write(cStart);
    serialPort.write(cLength);
    serialPort.write(cSubsteps);
    serialPort.write(cLoops);
    serialPort.write(byte(255));
    serialPort.write(byte(255));
    serialPort.write(byte(255));
    serialPort.write(byte(255));
    break;

    //send both
    case(25):
    println("SEND BOTH"); 
    serialPort.write(128+4); //STX sequencer notes + commands

    serialPort.write(cStart);
    serialPort.write(cLength);
    serialPort.write(cSubsteps);
    serialPort.write(cLoops);
    serialPort.write(nStart);
    serialPort.write(nLength);
    serialPort.write(nSubsteps);
    serialPort.write(nLoops);
    break;

    //bpm
    case(27):
    //calculate delay pr bpm
    println("BPM: "+bpmSynth);
    int delayPerBeat = 60000/bpmSynth/4;
    delayPerBeat = constrain(delayPerBeat,0,255);
    println("delay per beat: "+delayPerBeat);
    serialPort.write(128+5); //STX tempo commands
    serialPort.write(bpmSynth);
    serialPort.write(delayPerBeat);
    serialPort.write(255);  
    serialPort.write(255);
    serialPort.write(255);  
    serialPort.write(255);      
    break;
    
    //stop
    case(29):
    //calculate delay pr bpm
    println("BPM: 0");
    //delayPerBeat = constrain(delayPerBeat,0,255);
    //println("delay per beat: "+delayPerBeat);
    serialPort.write(128+5); //STX tempo commands
    serialPort.write(0);
    serialPort.write(255);
    serialPort.write(255);  
    serialPort.write(255);
    serialPort.write(255);  
    serialPort.write(255);      
    break;

    case(31): //tracker bpm has been updated
    calculateDelays();
    break;
    
    case(32): //start tracker
    calculateDelays();
    running = true;
    break;
    
    case(33): //stop tracker
    running = false;
    break;
    

  }
}

