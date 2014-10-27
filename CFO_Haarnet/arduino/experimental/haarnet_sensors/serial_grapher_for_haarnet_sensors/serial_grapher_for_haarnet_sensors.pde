/*
 ////////////////////////////////////////////////////////////////////////////////////
 serial monitor for CFO hårnet
 resonate 2014
 ////////////////////////////////////////////////////////////////////////////////////
 */

import processing.serial.*;

Serial myPort;

int rightMargin = 0;
int scrollStep = 1;

int expectedValues = 12;

boolean textOutput = false;
ArrayList graphers;

void setup () {

  size(200, 700);

  // List all the available serial ports
  printArray(Serial.list());

  // connect to arduino / serial port
  myPort = new Serial(this, Serial.list()[5], 9600);
  
  
  
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
  
  graphers = new ArrayList();
  
  colorMode(HSB,255);
  
  for (int i = 0; i<expectedValues; i++) {
    color thisColor = color(255/expectedValues*(i+1),255,255);
    float maxHeight = height/expectedValues;
    graphers.add (new Grapher(width, maxHeight*(i+1), -maxHeight, thisColor));
  }
  
  background(0);
}

void draw () {
  
  // nothing interesting here, all serial port actions happen in serialEvent() below
  
  // scroll image
  copy(scrollStep, 0, width-rightMargin, height, 0, 0, width-rightMargin, height);
}

void serialEvent (Serial myPort) {

  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    
    // split in to several string (if we send comma seperated values)
    String[] list = split(inString, ',');
    
    //if (textOutput) println("got "+list.length+" value(s)");
    
    for (int i=0; i<list.length; i++) {
      // trim off any whitespace:
      list[i] = trim(list[i]);
      if (textOutput) print(list[i]+",");
    }

    // draw the incoming values
    pushMatrix();
    translate(width-10,0);
    for (int i = 0 ; i<list.length ; i++) {
      //Grapher gr = (Grapher) graphers.get(i);
      //gr.update(map(float(list[i]),0,1023,0,1));
      
      translate(0,height/list.length);  
      float ysize = map(float(list[i]),0,1023,0,int(height/list.length));
      fill(255/list.length*(i+1),255,255);
      noStroke();
      rect(0,0,8,-ysize);
      
    }
    popMatrix();
    
  }
}

void keyPressed() {
  textOutput = !textOutput;
}

public class Grapher {
  public float minValue = 1, maxValue = 0;
  public float currentValue;
  public float xPos, yPos, ySize;
  public color graphColor;

  public Grapher(float theXPos, float theYPos, float theYSize, color theGraphColor) {
    xPos = theXPos;
    yPos = theYPos;
    ySize = theYSize;
    graphColor = theGraphColor;
  }
  
  void update(float currentValue) {
    pushMatrix();
    translate(xPos,yPos);
    fill(graphColor,150);
    noStroke();
    rect(0,0,0,map(currentValue,0,1,0,ySize));
    minValue = min(minValue,currentValue);
    maxValue = max(maxValue,currentValue);
    stroke(graphColor);
    point(0,map(minValue,0,1,0,ySize));
    point(0,map(maxValue,0,1,0,ySize));
    popMatrix();
  }

}


