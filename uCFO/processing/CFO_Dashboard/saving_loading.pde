byte[] savedKnobValues = new byte[numKnobs];
byte[] loadedKnobValues = new byte[numKnobs];


void loadKnobValues() {
  
  loadedKnobValues = loadBytes("knobValues.txt");

  for(int i=0; i<numKnobs; i++) {
    knobValue[i] = loadedKnobValues[i];
    controlP5.controller(controllerName[i]).setValue(knobValue[i]);
  }
  println();
  println("Loaded Knob Values");  
  
}

void saveKnobValues() {

    for(int i=0;i<numKnobs;i++) {
      savedKnobValues[i] = (byte)knobValue[i];
    }
    saveBytes("knobValues.txt", savedKnobValues);
    println();
    println("Saved Knob Values");
    
}

void sendMidiKnobs() {
  
  sendControlChange((byte)0, (byte)knobMidiCC[0],(byte)knobValue[0]);
  delay(delayTime);
  //semi1(knobValue[1]);
    int semi =(knobValue[1]+24)*2+16;
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[1], (byte)semi);

  delay(delayTime);
  //detune1(knobValue[2]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[2], (byte)knobValue[2]);

  delay(delayTime);
  //gain1(knobValue[3]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[3], (byte)knobValue[3]);

  delay(delayTime);
  //wave1(knobValue[4]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[4], (byte)(knobValue[4]*8));

  delay(delayTime);
  //fm1(knobValue[5]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[5], (byte)knobValue[5]);

  delay(delayTime);
  //freq2(knobValue[6]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[6], (byte)knobValue[6]);

  delay(delayTime);
  //semi2(knobValue[7]);
    semi = (knobValue[7]+24)*2+16;
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[7], (byte)semi);

  delay(delayTime);
  //detune2(knobValue[8]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[8], (byte)knobValue[8]);

  delay(delayTime);
  //gain2(knobValue[7]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[9], (byte)knobValue[9]);

  delay(delayTime);
  //wave2(knobValue[10]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[10], (byte)(knobValue[10]*8));
  
  delay(delayTime);
  //fm2(knobValue[11]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[11], (byte)knobValue[11]);

  delay(delayTime);
  //freq3(knobValue[12]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[12], (byte)knobValue[12]);

  delay(delayTime);
  //semi3(knobValue[13]);
    semi =(knobValue[13]+24)*2+16;
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[13], (byte)semi);

  delay(delayTime);
  //detune3(knobValue[14]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[14], (byte)knobValue[14]);

  delay(delayTime);
  //gain3(knobValue[15]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[15], (byte)knobValue[15]);

  delay(delayTime);
  //wave3(knobValue[16]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[16], (byte)(knobValue[16]*8));

  delay(delayTime);
  //fm3(knobValue[17]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[17], (byte)knobValue[17]);

  delay(delayTime);
  //attack(knobValue[18]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[18], (byte)knobValue[18]);

  delay(delayTime);
  //decay(knobValue[19]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[19], (byte)knobValue[19]);

  delay(delayTime);
  //sustain(knobValue[20]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[20], (byte)knobValue[20]);

  delay(delayTime);
  //release(knobValue[21]);
    if(!serialBusy) sendControlChange((byte)0, (byte)knobMidiCC[21], (byte)knobValue[21]);
  
  println();
  println("Sent Knob Values over MIDI");  

}


