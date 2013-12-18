String[] knobName = { "FREQ1",
                      "SEMI1",
                      "DETUNE1",
                      "GAIN1",
                      "WAVE1",
                      "FM1",
                      "FREQ2",
                      "SEMI2",
                      "DETUNE2",
                      "GAIN2",
                      "WAVE2",
                      "FM2",
                      "FREQ3",
                      "SEMI3",
                      "DETUNE3",
                      "GAIN3",
                      "WAVE3",
                      "FM3",
                      "ATTACK",
                      "DECAY",
                      "SUSTAIN",
                      "RELEASE"
                    };
                    
String[] controllerName = {   "freq1",
                              "semi1",
                              "detune1",
                              "gain1",
                              "wave1",
                              "fm1",
                              "freq2",
                              "semi2",
                              "detune2",
                              "gain2",
                              "wave2",
                              "fm2",
                              "freq3",
                              "semi3",
                              "detune3",
                              "gain3",
                              "wave3",
                              "fm3",
                              "attack",
                              "decay",
                              "sustain",
                              "release"
                            };
                              

                    
String[] waveform = { "SINE",
                      "SQUARE",
                      "PULSE",
                      "TRIANGLE",
                      "SAW",
                      "FUZZ",
                      "DIGI1",
                      "DIGI2",
                      "DIGI3",
                      "DIGI4",
                      "NOISE",
                      "DIGI6",
                      "TAN1",
                      "TAN2",
                      "TAN3",
                      "TAN4"
                    };
                    
Knob freq1,semi1,detune1,gain1,wave1,fm1,freq2,semi2,detune2,gain2,wave2,fm2,freq3,semi3,detune3,gain3,wave3,fm3,attack,decay,sustain,release;
Knob[] knobs = {freq1,semi1,detune1,gain1,wave1,fm1,freq2,semi2,detune2,gain2,wave2,fm2,freq3,semi3,detune3,gain3,wave3,fm3,attack,decay,sustain,release};

int[] knobMin =   {  0,-24,  0,  0,  0,  0,  0,-24,  0,  0,  0,  0,  0,-24,  0,  0,  0,  0,  0,  0,  0,  0};
int[] knobMax =   {127, 24,127,127, 15,127,127, 24,127,127, 15,127,127, 24,127,127, 15,127,127,127,127,127};
int[] knobValue = { 40,  0, 64,127,  2,  0, 40,  7, 64,127,  3,  0, 40,-12, 64,127,  5,  0, 12, 96, 64, 72};
int[] knobMidiCC= { 10, 11, 12, 13, 14, 15, 20, 21, 22, 23, 24, 25, 30, 31, 32, 33, 34, 35,114,115,116,117};
int numKnobs = knobValue.length;


int posX = 20;
int posY = 20;
int posEnvSpacer = 40;
int posW = 65;
int posH = 70;
int knobS = 40;


void setupKnobs() {

  controlP5 = new ControlP5(this);
  PFont p = createFont("Georgia",24); 
  controlP5.setControlFont(p,12);
  controlP5.setColorLabel(color(255,128));
/*
  Textlabel labelFreq = controlP5.addTextlabel("freqLabel","FREQ",posX+posW*0,posY-20);
  Textlabel labelSemi = controlP5.addTextlabel("semiLabel","SEMI",posX+posW*1,posY-20);
  Textlabel labelDetune = controlP5.addTextlabel("detuneLabel","DETUNE",posX+posW*2,posY-20);
  Textlabel labelGain = controlP5.addTextlabel("gainLabel","GAIN",posX+posW*3,posY-20);
  Textlabel labelWave = controlP5.addTextlabel("waveLabel","WAVE",posX+posW*4,posY-20);

  Textlabel labelAttack = controlP5.addTextlabel("attackLabel","A",posX+16+posW*0,posY+posH*4-20);
  Textlabel labelDecay = controlP5.addTextlabel("decayLabel","D",posX+16+posW*1,posY+posH*4-20);
  Textlabel labelsustain = controlP5.addTextlabel("sustainLabel","S",posX+16+posW*2,posY+posH*4-20);
  Textlabel labelReleasek = controlP5.addTextlabel("releaseLabel","R",posX+16+posW*3,posY+posH*4-20);

  Knob freq1 = controlP5.addKnob("freq1",    0,127,knobValue[0],  posX+posW*0,posY+posH*0,knobS);
  Knob semi1 = controlP5.addKnob("semi1",    -24,24,knobValue[1],  posX+posW*1,posY+posH*0,knobS);
  Knob detune1 = controlP5.addKnob("detune1",0,127,knobValue[2],  posX+posW*2,posY+posH*0,knobS);
  Knob gain1 = controlP5.addKnob("gain1",    0,127,knobValue[3], posX+posW*3,posY+posH*0,knobS);
  Knob wave1 = controlP5.addKnob("wave1",    0,15,knobValue[4],   posX+posW*4,posY+posH*0,knobS);
*/
  for(int i=0; i<18; i++) {
    knobs[i] = controlP5.addKnob(controllerName[i])
                      .setRange(knobMin[i],knobMax[i])
                      .setValue(knobValue[i])
                      .setPosition(posX+posW*(i%6),posY+posH*int(i/6))
                      .setRadius(knobS/2)
                      .setDragDirection(Knob.VERTICAL)
                      .setViewStyle(Knob.ELLIPSE)
                      .setNumberOfTickMarks(knobMax[i]-knobMin[i])
                      .snapToTickMarks(true)
                      .showTickMarks(false);
/*
    if(i%6==1) {
      knobs[i].setNumberOfTickMarks(49)
              .snapToTickMarks(true)
              .showTickMarks(false);
    }
    if(i%6==4) {
      knobs[i].setNumberOfTickMarks(16)
              .snapToTickMarks(true)
              .showTickMarks(false);
    }
*/
  }
  for(int i=18; i<22; i++) {
    knobs[i] = controlP5.addKnob(controllerName[i])
                      .setRange(knobMin[i],knobMax[i])
                      .setValue(knobValue[i])
                      .setPosition(posX+posW*(i%6),posY+posEnvSpacer+posH*int(i/6))
                      .setRadius(knobS/2)
                      .setDragDirection(Knob.VERTICAL)
                      .setViewStyle(Knob.ELLIPSE);
    
  }
/*
  Knob freq2 = controlP5.addKnob("freq2",    0,127,knobValue[5],  posX+posW*0,posY+posH*1,knobS);
  Knob semi2 = controlP5.addKnob("semi2",    -24,24,knobValue[6],  posX+posW*1,posY+posH*1,knobS);
  Knob detune2 = controlP5.addKnob("detune2",0,127,knobValue[7],  posX+posW*2,posY+posH*1,knobS);
  Knob gain2 = controlP5.addKnob("gain2",    0,127,knobValue[8], posX+posW*3,posY+posH*1,knobS);
  Knob wave2 = controlP5.addKnob("wave2",    0,15,knobValue[9],   posX+posW*4,posY+posH*1,knobS);

  Knob freq3 = controlP5.addKnob("freq3",    0,127,knobValue[10],  posX+posW*0,posY+posH*2,knobS);
  Knob semi3 = controlP5.addKnob("semi3",    -24,24,knobValue[11],  posX+posW*1,posY+posH*2,knobS);
  Knob detune3 = controlP5.addKnob("detune3",0,127,knobValue[12],  posX+posW*2,posY+posH*2,knobS);
  Knob gain3 = controlP5.addKnob("gain3",    0,127,knobValue[13], posX+posW*3,posY+posH*2,knobS);
  Knob wave3 = controlP5.addKnob("wave3",    0,15,knobValue[14],   posX+posW*4,posY+posH*2,knobS);

  Knob attack = controlP5.addKnob("attack",  0,127,knobValue[15],   posX+posW*0, posY+posH*4, knobS);
  Knob decay = controlP5.addKnob("decay",    0,127,knobValue[16],   posX+posW*1, posY+posH*4, knobS);
  Knob sustain = controlP5.addKnob("sustain",0,127,knobValue[17],   posX+posW*2, posY+posH*4, knobS);
  Knob release = controlP5.addKnob("release",0,127,knobValue[18],   posX+posW*3, posY+posH*4, knobS);
*/  
}

void writeKnobValues() {
  textAlign(CENTER);
  fill(255);
  text("FREQ",40,30);
  text("SEMI",100,30);
  text("DETUNE",160,30);
  text("GAIN",220,30);
  text("WAVE",280,30);
  
  text("A",40,270);
  text("D",100,270);
  text("S",160,270);
  text("R",220,270);

  
  fill(128);
  text(knobValue[0],40,90);
  text(knobValue[1],100,90);
  text(knobValue[2],160,90);
  text(knobValue[3],220,90);
  text(waveform[knobValue[4]],280,90);
  text(knobValue[5],40,150);
  text(knobValue[6],100,150);
  text(knobValue[7],160,150);
  text(knobValue[8],220,150);
  text(waveform[knobValue[9]],280,150);
  text(knobValue[10],40,210);
  text(knobValue[11],100,210);
  text(knobValue[12],160,210);
  text(knobValue[13],220,210);
  text(waveform[knobValue[14]],280,210);
  text(knobValue[15],40,330);
  text(knobValue[16],100,330);
  text(knobValue[17],160,330);
  text(knobValue[18],220,330);  
}

void freq1(int val)
{
  int knob = 0;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + (int)(pow(2,(knobValue[knob]-69)/12.0)*440) + " Hz");
    //(2^((p-69)/12))*440
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void semi1(int val)
{
  int knob = 1;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    int semi =(knobValue[knob]+24)*2+16;
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)semi);
  }
}

void detune1(int val)
{
  int knob = 2;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void gain1(int val)
{
  int knob = 3;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void wave1(int val)
{
  int knob = 4;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob] + " (" + waveform[knobValue[knob]] + ")");
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)(knobValue[knob]*8));
  }
}

void fm1(int val)
{
  int knob = 5;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void freq2(int val)
{
  int knob = 6;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + (int)(pow(2,(knobValue[knob]-69)/12.0)*440) + " Hz");
    //(2^((p-69)/12))*440
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void semi2(int val)
{
  int knob = 7;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    int semi =(knobValue[knob]+24)*2+16;
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)semi);
  }
}

void detune2(int val)
{
  int knob = 8;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void gain2(int val)
{
  int knob = 9;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void wave2(int val)
{
  int knob = 10;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob] + " (" + waveform[knobValue[knob]] + ")");
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)(knobValue[knob]*8));
  }
}

void fm2(int val)
{
  int knob = 11;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void freq3(int val)
{
  int knob = 12;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + (int)(pow(2,(knobValue[knob]-69)/12.0)*440) + " Hz");
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void semi3(int val)
{
  int knob = 13;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    int semi =(knobValue[knob]+24)*2+16;
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)semi);
  }
}

void detune3(int val)
{
  int knob = 14;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void gain3(int val)
{
  int knob = 15;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void wave3(int val)
{
  int knob = 16;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob] + " (" + waveform[knobValue[knob]] + ")");
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)(knobValue[knob]*8));
  }
}

void fm3(int val)
{
  int knob = 17;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void attack(int val)
{
  int knob = 18;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void decay(int val)
{
  int knob = 19;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void sustain(int val)
{
  int knob = 20;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}

void release(int val)
{
  int knob = 21;
  if(knobValue[knob] != val) {
    knobValue[knob] = val;
    if(printChange) println(knobName[knob] + " is: " + knobValue[knob]);
    sendControlChange((byte)0, (byte)knobMidiCC[knob], (byte)knobValue[knob]);
  }
}


