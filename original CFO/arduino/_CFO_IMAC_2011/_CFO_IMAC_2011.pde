//          _             _       _    _            _                   _                  _          _                 _                       _            _          _            _          
//        /\ \           / /\    / /\ /\ \         / /\                /\ \               /\ \       / /\              /\ \         _          /\ \         /\ \       /\ \         /\ \     _  
//       /  \ \         / / /   / / //  \ \       / /  \              /  \ \             /  \ \     / /  \             \_\ \       /\ \       /  \ \       /  \ \     /  \ \       /  \ \   /\_\
//      / /\ \ \       / /_/   / / // /\ \ \     / / /\ \            / /\ \ \           / /\ \ \   / / /\ \            /\__ \   ___\ \_\     / /\ \ \     / /\ \ \   / /\ \ \     / /\ \ \_/ / /
//     / / /\ \ \     / /\ \__/ / // / /\ \_\   / / /\ \ \          / / /\ \_\         / / /\ \_\ / / /\ \ \          / /_ \ \ /___/\/_/_   / / /\ \ \   / / /\ \_\ / / /\ \_\   / / /\ \___/ / 
//    / / /  \ \_\   / /\ \___\/ // /_/_ \/_/  / / /  \ \ \        / / /_/ / /        / /_/_ \/_// / /  \ \ \        / / /\ \ \\__ \/___/\ / / /  \ \_\ / / /_/ / // /_/_ \/_/  / / /  \/____/  
//   / / /    \/_/  / / /\/___/ // /____/\    / / /___/ /\ \      / / /__\/ / _      / /____/\  / / /___/ /\ \      / / /  \/_/  /\/____\// / /   / / // / /__\/ // /____/\    / / /    / / /   
//  / / /          / / /   / / // /\____\/   / / /_____/ /\ \    / / /_____//\_\    / /\____\/ / / /_____/ /\ \    / / /         \ \_\   / / /   / / // / /_____// /\____\/   / / /    / / /    
// / / /________  / / /   / / // / /______  / /_________/\ \ \  / / /       \/ /\  / / /      / /_________/\ \ \  / / /           \/_/  / / /___/ / // / /      / / /______  / / /    / / /     
/// / /_________\/ / /   / / // / /_______\/ / /_       __\ \_\/ / /        /_/ / / / /      / / /_       __\ \_\/_/ /                 / / /____\/ // / /      / / /_______\/ / /    / / /      
//\/____________/\/_/    \/_/ \/__________/\_\___\     /____/_/\/_/         \_\/  \/_/       \_\___\     /____/_/\_\/                  \/_________/ \/_/       \/__________/\/_/     \/_/       
//         

//   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   + CHEAP, FAT and OPEN                                                          +
//   + an open source platform for musical exploration, composition and performance +
//   + by jacob sikker remin                                                        +
//   + re:new / IMAC 2011 - the unheard avantgarde revision - may 2011              +
//   + project home: http://8bitklubben.dk/project/CFO/                             +
//   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// sound processing based on the arduino pocket piano code and circuit, by critter and guittari
// more info: http://www.critterandguitari.com/home/store/arduino-piano.php

// GENERAL OPTIMIZATION NOTE
// labelled code where i see abvious places for optimization
// OPTIMIZE (code works, but messy), TO CHECK (something might be up here), NEW THING (new feature, nice to have), SETTING (add this variable to settings menu)

// TODO
// stylophone octave is taken over by sequencer / memory sequence
// these should be seperated, so that one can still jam with the sequence in the expected octave
// IDEAS
// timing issues: 
// gate sequencer should be able to sync with processing + have its own time
// substeps
// memory sequencer: UP / DOWN could double / half the memory sequence length

#include <stdio.h>                      // lets us do some C string manipulations for the LCD display
#include <avr/pgmspace.h>               // lets us use progmem for the giant wavetables
#include <LiquidCrystal.h>              // LCD display library
#include <EEPROM.h>                     // lets us read from and write to the EEPROM

// LCD setup
LiquidCrystal lcd(16, 17, 3, 5, 6, 7);  // LiquidCrystal(rs, enable, d4, d5, d6, d7) 
char printMsg[17];                      // 16 chars plus end line char
char oldPrintMsg1[17];                  // i guess this could be a two dimensional array... OPTIMIZE?
char oldPrintMsg2[17];
int delayLCD = 100;
unsigned long lastLCDUpdate = millis();

// rotary encoder
#define encoder0PinA  2 
#define encoder0PinB  12
volatile unsigned int encoder0Pos = 0;
unsigned int oldEncoder0Pos = encoder0Pos;
int encoderChange = 0;

// sound generation
int frequency = 0;
byte gain = 0xff;                       // gain of oscillator
byte waveForm = 0;
byte modulatorWaveForm = 0;
char* waveName[] = {"sine","square","pulse","triangle","sawtooth","fuzzsquare","digiwave 1","digiwave 2","digiwave 3","digiwave 4","digiwave 5","digiwave 6",
  "tanwave 1","tanwave 2","tanwave 3","tanwave 4"};
#define numberOfWaveforms 16
// FM synthesis // research should go in to this! OPTIMIZE // NEW THING
uint16_t harmonicity = 0;              // harmonicity (first 8 bits are fractional)
uint8_t modulatorDepth = 0;            // modulation depth (0 - 255)

// define keynames, and which octave they belong to
char* noteNames[] = {"A ", "A#", "B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "H ", "C ", "C#", "D ", "D#", "E "};
int octaveAdd[20] = {-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1};
// hertz values for each stylophone key
int hertzTable[20] = {370,392,415,440,466,494,523,555,587,622,659,699,740,784,831,880,932,988,1047,1109};
int octave = 0;                       //current base octave
//int sequencerOctave = 0;              //current octave of sequencer

//button input, through resistor ladder connected to analog input 0
int buttonVal[6] = {161, 325, 492, 662, 838, 1023};
#define MODE 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define OK 4
#define UP 5
int buttonNoise = 25;                 //is suddenly superhigh? TO CHECK!
int activeButton = -1;                
int oldActiveButton = activeButton;
boolean buttonPushed = false;         //this could be redundant if -1 was used for non-active. OPTIMIZE

//stylophone input, through resistor ladder connected to analog input 5
unsigned int styloVal[20] PROGMEM = {53, 101, 148, 193, 238, 282, 327, 372, 417, 462, 509, 557, 606, 657, 710, 765, 824, 886, 952, 1022};
int styloNoise = 25;                  //noise threshold on analog input pin, might be different on non-fixed PCBs. TO CHECK
int activeKey = -1;
int oldActiveKey = activeKey;
boolean styloMapping = true;

// setup up shift register
int latchPin = 8;
int clockPin = 4;
int dataPin = 15;
// a byte keeps 8 bits
byte data1 = 0;
byte data2 = 0;
// (NOTE: an int keeps 16, so we should be able to keep all info in one int. OPTIMIZE)
// bools to keep LED status. this is redundant as we keep the info in data1/2 already. OPTIMIZE.
boolean LED[16] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
//#define LEDFrequency = 200 //update LEDs ever 200 milliseconds, or 50 times a second --- are LEDs updated every cycle now? TO CHECK

//general navigation and logic
long lastLEDUpdate = millis();
int mode = 0;
int oldMode = mode;
#define numberOfModes 4
char* modeName[] = {"stylophone", "modulation", "sequencer", "settings"};
int pitchBend;
#define STYLOPHONE 0
#define MODULATION 1
#define SEQUENCER 2
#define SETTINGS 3

//char* settingsName[] = {"decay", "master", "slave"}; lets make it stupid stupid
int activeSetting = 0;

//analog inputs
#define BUTTONS 0
#define STYLOPHONE_INPUT 5
#define EXPRESSION 4

//gate sequencer
int counter = 0;
int tempo = 100;                       //should be BPM, right now it is just a delay. OPTIMIZE
unsigned long lastBeat = millis();
boolean gate[16] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true}; //same as LED? redundant. OPTIMIZE
boolean sequenceBlink = false;
boolean editing = false;
#define LEDBlink 50                    //length of each sequencer blink

unsigned long lastKeyPress = millis();
#define waitForKeypress 50             //pause for each keypress

#define notTouched 60000               //go to instructions mode if not touched for X ms
boolean instructionsMode = false;      
unsigned long lastUse = millis();
char* instructionText[] = {"hello", "how to play:", "connect cable", "to piano keys", "use push keys", "and knob", "to edit sound", "and sequencer", "enjoy!"};
#define instructionsSpeed 2000         //each instruction on screen for X ms
int instruction = 0;
int oldInstruction = -1;
int numberOfInstructions = 9;          //this could be checked automatically. OPTIMIZE.
boolean demoProne = true;              //set to true for demo mode when inactive

//sequencer
int bpm = 160;
boolean looping = false;

int memNoteStart = 0, memNoteEnd = 3, memNoteSubsteps = 4, memNotePos = 0, memCommandStart = 0, memCommandEnd = 3, memCommandSubsteps = 4, memCommandPos = 0;
int sequenceStart = 0, sequencePosition = sequenceStart, sequenceLength = 16, sequenceSubsteps = 4;

unsigned long lastNote, lastCommand;
unsigned long lastSequenceStep = millis();
int millisPerNote, millisPerCommand;
int delayPerBeat = 100;

int lastNoteSent, lastOctave, lastCommandSent, lastCommandValue; // could be bytes? OPTIMZE
int memNoteLoops, memCommandLoops;

int millisPerSequenceStep = 60000/bpm/4;

boolean master = true, slave = false;

//music table: 4 bytes/step: NOTE, OCTAVE, COMMAND, COMMAND VALUE
prog_uchar musicTable[] PROGMEM = {
0,1,255,255,255,255,255,255,255,255,255,255,255,255,255,255,4,255,255,255,7,255,255,255,255,255,255,255,0,2,255,255,0,0,255,255,255,255,215,1,255,255,255,255,255,255,255,255,9,255,215,0,0,255,255,255,255,255,255,255,255,255,255,255,
7,3,215,12,7,1,215,3,0,2,215,11,7,3,215,3,0,1,215,12,0,2,215,3,10,2,215,13,0,1,215,3,0,1,215,12,10,2,215,3,0,1,215,11,0,1,215,3,5,1,215,13,0,1,215,3,0,2,215,12,5,1,215,3,8,1,215,11,0,2,215,3,5,2,215,11,8,1,215,3,7,1,215,13,5,2,215,3,10,2,215,12,7,1,215,3,5,2,215,12,10,2,215,3,3,2,215,13,5,2,215,3,10,2,215,12,3,2,215,3,7,1,215,13,10,2,215,3,
7,4,215,11,7,1,215,3,0,2,215,255,7,4,215,3,0,1,215,11,0,2,215,3,10,2,215,255,0,1,215,3,0,1,215,2,10,2,215,3,0,1,215,4,0,1,215,3,5,3,215,6,0,1,215,3,0,2,215,7,5,3,215,3,8,1,215,9,0,2,215,3,5,1,215,8,8,1,215,3,7,1,215,7,5,1,215,3,10,2,215,9,7,1,215,3,5,4,215,6,10,2,215,3,3,2,215,9,5,4,215,3,9,2,215,8,3,2,215,3,7,1,215,6,9,2,215,3,
7,1,215,12,8,4,215,3,0,3,215,11,7,1,215,3,3,2,215,12,0,3,215,3,5,1,215,13,3,2,215,3,7,2,215,1,5,1,215,3,7,2,215,11,7,2,215,3,5,1,215,5,7,2,215,3,8,1,215,12,5,1,215,3,0,2,215,11,8,1,215,3,0,2,215,8,0,2,215,3,10,1,215,13,0,2,215,3,0,1,215,9,10,1,215,3,0,1,215,10,0,1,215,3,3,1,215,11,0,1,215,3,0,1,215,15,3,1,215,3,8,4,215,4,0,1,215,3,
7,1,215,12,8,4,215,3,0,3,215,8,7,1,215,3,3,4,215,8,0,3,215,3,5,1,215,13,3,4,215,3,7,2,215,1,5,1,215,3,2,4,215,11,7,2,215,3,5,1,215,5,2,4,215,3,8,1,215,12,5,1,215,3,0,2,215,11,8,1,215,3,0,2,215,8,0,2,215,3,10,3,215,7,2,4,215,7,0,1,215,9,2,4,215,3,0,4,215,10,0,1,215,3,3,1,215,11,0,4,215,3,0,1,215,15,3,1,215,3,8,4,215,4,0,1,215,3,
7,3,215,12,7,1,215,3,10,3,215,11,7,3,215,3,0,1,215,12,10,3,215,3,10,2,215,13,0,1,215,3,3,1,215,1,10,2,215,3,0,1,215,11,2,1,215,3,5,1,215,5,0,1,215,3,0,2,215,12,5,1,215,3,10,1,215,11,0,2,215,3,5,2,215,8,10,1,215,3,3,1,215,13,5,2,215,3,8,2,215,9,3,1,215,3,5,2,215,10,7,2,215,3,7,3,215,11,5,2,215,3,10,2,215,15,7,3,215,3,7,1,215,11,10,2,215,3,
9,1,215,1,0,2,215,2,255,255,255,255,0,0,215,9,255,255,255,255,3,255,255,255,4,255,194,100,255,255,255,255,9,255,255,255,255,255,255,255,0,0,215,1,255,255,255,255,1,255,194,20,255,255,255,255,255,255,255,255,255,255,255,255,  
0,0,199,255,4,0,215,0,255,255,215,0,255,255,215,0,2,255,215,3,9,255,215,9,2,1,215,9,9,255,255,255,9,1,199,255,4,255,255,255,5,0,199,255,9,255,199,200,11,1,199,255,9,255,215,4,9,255,203,255,9,255,207,63,
2,0,207,62,4,1,199,255,0,0,203,255,0,1,199,255,0,0,203,255,5,1,199,255,0,0,203,255,0,0,255,255,0,1,199,10,0,0,231,114,0,1,231,114,0,0,231,114,0,1,231,114,0,0,231,14,0,1,231,14,0,0,231,14,
0,3,194,10,0,4,255,255,0,3,255,255,0,4,255,255,0,3,194,20,0,4,255,255,0,3,255,255,0,4,255,255,0,3,194,40,0,4,255,255,0,3,255,255,0,4,255,255,0,3,194,80,0,4,255,255,0,3,255,255,0,4,255,255,
0,4,194,255,0,3,194,255,0,4,215,3,0,3,199,255,0,4,255,255,0,3,255,255,0,4,255,255,0,3,255,255,0,4,255,255,0,3,255,255,0,4,255,255,0,3,255,255,0,4,255,255,0,3,255,255,0,4,194,1,0,3,255,255,  
0,0,194,100,0,2,255,255,0,0,255,255,0,2,255,255,0,0,194,10,0,2,255,255,0,0,255,255,0,2,255,255,0,0,194,255,0,2,255,255,0,0,255,255,0,2,255,255,0,0,194,1,0,2,255,255,0,0,255,255,0,2,255,255,
2,0,207,64,4,1,199,255,0,0,203,255,0,1,199,255,0,0,203,255,5,1,199,255,0,0,203,255,0,0,255,255,0,1,199,10,0,0,231,114,0,1,231,114,0,0,231,114,0,1,231,114,0,0,231,14,0,1,231,14,0,0,231,14 //elevatormusik, 16steps
};

int musicTableLength = 512; // should be automated. TO CHECK / FIX

byte ID = 0;                // ID of this specific unit

unsigned long killTime; // added to be able to kill notes -- not working very well -- OPTIMIZE
boolean kill = false;

//tracker idle
unsigned long idle = millis();
#define idletime 800
boolean isIdle = false;

//setup decay
int sustain = 100; //in ms
byte decaySteps = 10, decaySpeed = 50, endDecay = 0;
boolean decaying = false;
unsigned long lastDecay = millis();
byte startAttack = 255, endAttack = 255, attackSteps = 50, attackSpeed = 1;
boolean attacking = true;
unsigned long lastAttack = millis();

boolean gateOn = true;

unsigned long lastSequenceEdit = millis();
#define sequenceEditTime 50

boolean takeStep = false;

void setup()
{

  
  //ID in EEPROM is useful if you have several units, and want to address them seperately via serial
  /*
  EEPROM.write(0,0);                   // uncomment this when running first time, replace second number with wanted ID (0-255), then comment again
  // read ID of this unit
  ID = EEPROM.read(0);
  sprintf(printMsg,"ID: %3d",ID);
  printLCD(printMsg,0);
  */
  
  //setup shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  Serial.begin(2400);                  // 2400 baud is max speed, since we are communicating via IR light on a 38kHz carrier frequency
  
  cli();                               // clear interrupts. this has been added, to get the pocket piano code working
  
  // arduino pocket piano code follows
  // more info on the pocket piano here: http://www.critterandguitari.com/home/store/arduino-piano.php
  // set up syntheziser
  // this is the timer 2 audio rate timer, fires an interrupt at 15625 Hz sampling rate
  
  TIMSK2 = 1<<OCIE2A;                      // interrupt enable audio timer
  OCR2A = 127;
  TCCR2A = 2;                              // CTC mode, counts up to 127 then resets
  TCCR2B = 0<<CS22 | 1<<CS21 | 0<<CS20;    // different for atmega8 (no 'B' i think)
  SPCR = 0x50;                             // set up SPI port
  SPSR = 0x01;
  DDRB |= 0x2E;                            // PB output for DAC CS, and SPI port
                                           // DAC SCK is digital 13, DAC MOSI is digital 11, DAC CS is digital 10
  PORTB |= (1<<2);                         // not exactly sure what this does. TO CHECK
 
  // rotary encoder interrupt
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // internal pullup resistor high
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // internal pullup resistor high
  attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
  
  //create 38kHz carrier wave for IR serial
  pinMode(9, OUTPUT);
  
  bitWrite(TCCR1A, WGM10, 0);            // Clear Timer on Compare Match (CTC) Mode
  bitWrite(TCCR1A, WGM11, 0);
  bitWrite(TCCR1B, WGM12, 1);
  bitWrite(TCCR1B, WGM13, 0);

  bitWrite(TCCR1A, COM1A0, 1);          // Toggle OC1A and OC1B on Compare Match.
  bitWrite(TCCR1A, COM1A1, 0);

  bitWrite(TCCR1B, CS10, 1);            // No prescaling
  bitWrite(TCCR1B, CS11, 0);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
  bitWrite(TCCR1B, CS12, 0);

  OCR1A = 210;
  OCR1B = 210;
  
  // global interrupt enable
  sei();     
  
  startupAnim();
}

//****************************************************************************************** MAIN LOOP

void loop() { // IN GENERAL: loop() has too much code in it! OPTIMIZE
  //read expression control
  int expressionRead = analogRead(EXPRESSION);
  pitchBend = expressionRead * 2;      // multiplication amount should be tweakable. SETTING.
  //check for keypresses
  if (millis() > lastKeyPress+waitForKeypress) readButtons();
  //read stylophone input  
  int styloRead = analogRead(STYLOPHONE_INPUT);

  // attack / sustain / release hack
  

  if (attacking && (millis() > lastAttack + attackSpeed)) {
//    sprintf(printMsg, "attacking: %3d", gain);
//    printLCD(printMsg,1);  
    lastAttack = millis();
    if (gain < (endAttack-attackSteps)) {
      gain = gain + attackSteps;
    } else {
      gain = endAttack;
      attacking = false;
      //decaying = true;
    }
    //sprintf(printMsg, "gain: %3d", gain);
    //printLCD(printMsg,1);  
  }
  
  // start decay when sustain is over
  if (millis() > lastNote + sustain && styloRead < styloNoise) {
    decaying = true;
  }
  
  if (decaying && (millis() > lastDecay + decaySpeed)) {
//    sprintf(printMsg, "decaying: %3d", gain);
//    printLCD(printMsg,1); 
    lastDecay = millis();
    if (gain > (endDecay+decaySteps)) {
      gain = gain - decaySteps;
    } else {
      gain = endDecay;
      decaying = false;
    }
    //sprintf(printMsg, "gain: %3d", gain);
    //printLCD(printMsg,1);  
  }

  
  if (instructionsMode && demoProne) {
    displayInstructions();
  } else {  
    //the code above in loop() should not be needed. except reading the inputs. OPTIMIZE.
    switch (mode) {
      case STYLOPHONE:
        //stylophone / gate sequencer edit mode
        checkEncoder();
        if (encoderChange != 0) {
          tempo += encoderChange*5;
          sprintf(printMsg, "delay: %3d", tempo);
          printLCD(printMsg,0);     
          instructionsMode = false;
        }
        
        gateSequencerKeyCheck();     // check for keypresses

        if (editing) {
          gateSequencerInput();      // edit gate sequencer, this is super unprecise. OPTIMIZE.
        } else {
          stylophoneInput();         // jam that stylophone!
        }
        break;
      
      case MODULATION:
        //modulator mode
        modulationKeyCheck();
        stylophoneInput();
        break;

      case SEQUENCER:
        //sequencer mode
        sequencerKeyCheck();
        stylophoneInput();
        break;
        
      case SETTINGS:
        //tweak them settings
        switch(activeSetting) {
          case 0: // change attack steps
            checkEncoder();
            attackSteps += encoderChange;
            attackSteps = constrain (attackSteps, 0, 255);
            sprintf(printMsg, "attackSteps: %3d", attackSteps);
            printLCD(printMsg,1);
            break;
          case 1: // attack speed
            checkEncoder();
            attackSpeed += encoderChange;
            attackSpeed = constrain (attackSpeed, 0, 255);
            sprintf(printMsg, "attackSpeed: %3d", attackSpeed);
            printLCD(printMsg,1);
            break;
          case 2: // attack speed
            checkEncoder();
            sustain += encoderChange*5;
            sustain = max(sustain, 0);
            sprintf(printMsg, "sustain(ms):%4d", sustain);
            printLCD(printMsg,1);
            break;
          case 3: // change decay steps
            checkEncoder();
            decaySteps += encoderChange;
            decaySteps = constrain (decaySteps, 0, 255);
            sprintf(printMsg, "decaySteps: %3d", decaySteps);
            printLCD(printMsg,1);
            break;
          case 4: // decay speed
            checkEncoder();
            decaySpeed += encoderChange;
            decaySpeed = constrain (decaySpeed, 0, 255);
            sprintf(printMsg, "decaySpeed: %3d", decaySpeed);
            printLCD(printMsg,1);
            break;
          case 5: // decay speed
            checkEncoder();
            startAttack += encoderChange;
            startAttack = constrain (startAttack, 0, 255);
            sprintf(printMsg, "attack gain: %3d", startAttack);
            printLCD(printMsg,1);
            break;
          case 6: // decay speed
            checkEncoder();
            endDecay += encoderChange;
            endDecay = constrain (endDecay, 0, 255);
            sprintf(printMsg, "end gain: %3d", endDecay);
            printLCD(printMsg,1);
            break;            
          case 7: // master/slave setting
            checkEncoder();          
            if (encoderChange == 1) { 
              master = true;       
              slave = false;
            }
            if (encoderChange == -1) {
              slave = true;
              master = false;
            }
            if (master) {
              printLCD("sync: master", 1);
            } else {
              printLCD("sync: slave", 1);
            }
            break;
          
        }
        settingsKeyCheck();
        stylophoneInput();
        break;
    }    
    
  }

  updateGateSequencer();
  checkModeChange();
  
  if (looping) sequencer();
  
  if (Serial.available() >= 6) parseSerial();
  if (slave && Serial.available()) serialReadIR();

  if (!instructionsMode && millis() > lastUse + notTouched) {
    instructionsMode = true;
    instruction = 0;
  }
  
  //allow super short notes with kill command
//  if (kill && (killTime < millis())) {
//    gain = 0;
//    kill = false;
//    printLCD("argh!",0);
//  }
}

void startupAnim() {
  //bling bling
  lcd.begin(16,2);
  
  createCharacters();
  
  lcd.setCursor(13, 0);
  lcd.write(0);
  lcd.setCursor(13, 1);
  lcd.write(1);
  lcd.setCursor(14, 0);
  lcd.write(2);
  lcd.setCursor(14, 1);
  lcd.write(3);
  lcd.setCursor(15, 0);
  lcd.write(4);
  lcd.setCursor(15, 1);
  lcd.write(5);
  
  lcd.setCursor(0,0);
  lcd.print("CHEAP,FAT");
  lcd.setCursor(0,1);
  lcd.print("and OPEN");
  
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("let's go!");
}

void serialReadIR() {
  char val = Serial.read();
  
//  sprintf(printMsg, "incoming IR: %1c", val);
//  printLCD(printMsg,1);
  
  if (val == 'R') {
    counter = 0;
    lastBeat = millis();
    sequenceBlink = true; 
  }
  if (val == 'S') {
    counter++;
    counter = counter%16;
    lastBeat = millis();
    sequenceBlink = true;
  } 
}

void settingsKeyCheck() {
  if (buttonPushed) {
    switch (activeButton) {
      case MODE:
        mode++;
        break;
      case DOWN:
        activeSetting++;
        break;
      case UP:
        activeSetting --;
        activeSetting = constrain(activeSetting,0,255); // SET TO MAX NR OF SETTINGS
        break;
        
    }
  }
  buttonPushed = false;
}

void displayInstructions () {
    //instructionsmode
    instruction = (millis() - lastUse) / instructionsSpeed;
    instruction = instruction % numberOfInstructions;
    if (instruction != oldInstruction) {
      printLCD ("CHEAP,FAT+OPEN",0);
      printLCD (instructionText[instruction],1);
      oldInstruction = instruction;
    }
    stylophoneInput();
}

void gateSequencerInput () {                        //read stylophone input, map to stylophone keys and flip corresponding sequencer gate
  
  int styloRead = analogRead(STYLOPHONE_INPUT);
  
  
  
  if (styloRead > styloNoise) {
    lastUse = millis();
    activeKey = stylophoneMapping (styloRead);  // map analog input to stylophone keys
    
    if (activeKey != oldActiveKey) {
      if (millis() > lastSequenceEdit + sequenceEditTime) {
        gate[activeKey] = !gate[activeKey];       // flip sequencer gate
        oldActiveKey = activeKey;
        lastSequenceEdit = millis();
      }
    }
    
  } else {
    activeKey = -1;
  }  
}


void gateSequencerKeyCheck () {
  if (buttonPushed) {
    switch (activeButton) {
      case DOWN:
        octave--;
        sprintf(printMsg, "octave:%3d", octave);
        printLCD(printMsg,0);       
        break;
      case LEFT:
        waveForm--;
        waveForm = waveForm%numberOfWaveforms;    
        printLCD(waveName[waveForm],0); 
        break;
      case OK:
        if (editing) {
          printLCD("rock it!",0);
          editing = false;
          //send gate data over IR
          //Serial.print(data1);
          //Serial.print(data2);
        } else {
          printLCD("edit sequence",0);
          editing = true;
        }
        break;
      case RIGHT:
        waveForm++;
        waveForm = waveForm%numberOfWaveforms;
        //sprintf(printMsg, "waveform: %3d", waveForm);
        printLCD(waveName[waveForm],0); 
        break;
      case UP:
        octave++;
        sprintf(printMsg, "octave:%3d", octave);
        printLCD(printMsg,0);  
        break;  
      case MODE:
        if (editing) {
          printLCD("reset sequence",0);
          data1 = 255;
          data2 = 255;
          for (int i=0; i<8; i++) {
            gate[i] = bitRead (data1, i);
            gate[i+8] = bitRead (data2, i);
          }
          editing = false;
        } else {
          mode++;
        }
        break;
    }
  buttonPushed = false;
  }
}

void updateGateSequencer() {

  if (millis()>(lastBeat+tempo) && master) {
    counter++;
    counter = counter%16;
    lastBeat = millis();
    sequenceBlink = true;
    
    //send IR sync character to other slave units
    if (counter == 0) {
      //reset
      Serial.print('R');
    } else {
      //step
      Serial.print('S');
    }
  }
  

  if (gate[counter]) {
    gateOn = true;
  } else {
    gateOn = false;
  }

  for (int i = 0; i<16; i++) {
    LED[i] = gate[i];
  }
  
  //inverse LED at sequence time
  if (sequenceBlink) {
    if (millis()<lastBeat+LEDBlink) {
      LED[counter] = !LED[counter];
    } else {
      sequenceBlink = false;
    }
  }
  
  //send LED data to shift registers
  compileBytes();    
  sendLEDData(data1,data2);

}

// buttons: down, left, ok, right, up, home
void modulationKeyCheck () {
      if (buttonPushed) {
        switch (activeButton) {
          case LEFT:
            harmonicity--;
            sprintf(printMsg, "rate: %3d", harmonicity);
            printLCD(printMsg,0); 
            break; 
          case DOWN:
            modulatorDepth --;
            modulatorDepth = modulatorDepth%255;
            sprintf(printMsg, "depth: %3d", modulatorDepth);
            printLCD(printMsg,0); 
            break; 
          case OK:
            //cycle through modulation waveforms
            modulatorWaveForm ++;
            modulatorWaveForm = modulatorWaveForm %numberOfWaveforms;
            sprintf(printMsg, "m:%14s", waveName[modulatorWaveForm]);
            printLCD(printMsg,0);
            break;
          case UP:
            modulatorDepth ++;
            modulatorDepth = modulatorDepth%255;
            sprintf(printMsg, "depth: %3d", modulatorDepth);
            printLCD(printMsg,0); 
            break; 
          case RIGHT:
            harmonicity++;
            sprintf(printMsg, "rate: %3d", harmonicity);
            printLCD(printMsg,0); 
            break;  
          case MODE:
            mode++;
            break;
        }
      buttonPushed = false;
    }
}


void sequencerKeyCheck() {
  boolean printed = false;
  if (buttonPushed) {
    checkEncoder();
    
    switch (activeButton) {
      case LEFT: // change start adress of memory sequence
        if (encoderChange != 0) {
          sequenceStart = constrain (sequenceStart+encoderChange, 0, musicTableLength);
        }
        break;
      case RIGHT: // change end adress of memory sequence
        if (encoderChange != 0) {
          sequenceLength = constrain (sequenceLength+encoderChange, 0, musicTableLength);
        }
        break;
      case OK:   //change sequencer mode
        looping = !looping;
        break;
      case UP:   //change bpm
        if (encoderChange != 0) {
          bpm = max(bpm+encoderChange*5,0);
          millisPerSequenceStep = 60000/bpm/4;
          sprintf(printMsg,"bpm: %3d",bpm);
          printLCD(printMsg,0);
          printed = true;
        }
        break;
      case DOWN: //change sustain
        if (encoderChange != 0) {
          sustain = max(sustain+encoderChange*5,1);
          sprintf(printMsg,"sustain: %3d",sustain);
          printLCD(printMsg,0);
          printed = true;
        }
        break;
      case MODE:
        mode++;
        break;
    }
    if (looping) sprintf(printMsg,"   loop: %3d %3d",sequenceStart,sequenceLength);
    if (!looping) sprintf(printMsg,"no loop: %3d %3d",sequenceStart,sequenceLength);
    printLCD(printMsg,1);
    
  }
  buttonPushed = false;
}

// define serial (pseudo midi) commands
#define INSTRUMENT_CHANGE 128+73  //'I'
#define NOTE_SET 128+78           //'N'
#define KILL_NOTE 128+75          //'K'
#define GAIN_ABS 128+71           //'G'
#define GAIN_REL 128+103          //'g'
#define OCTAVE_ABS 128+79         //'O'
#define OCTAVE_REL 128+111        //'o'
#define WAVE 128+87               //'W'
#define TEMPO 128+66              //'B'

void sequencer() {
  //printLCD("playing from memory",1);
  if (millis() - lastSequenceStep > millisPerSequenceStep) {
    byte note, octave_value, command, command_value;
    int readPosition = sequencePosition*4;
    note =  pgm_read_byte_near(musicTable + readPosition); 
    
    readPosition++;
    octave_value = pgm_read_byte_near(musicTable + readPosition); 
    
    readPosition++;
    command =  pgm_read_byte_near(musicTable + readPosition); 
    
    readPosition++;
    command_value = pgm_read_byte_near(musicTable + readPosition); 
    
    sprintf(printMsg,"M%3d%3d%3d%3d%3d",sequencePosition,note,octave_value,command,command_value);
    printLCD(printMsg,1);
    
    trackerCommand(note,octave_value,command,command_value);
    sequencePosition++;
    if (sequencePosition >= sequenceStart+sequenceLength) sequencePosition = sequenceStart;
    
    lastSequenceStep = millis();
    
  }
}

void parseSerial() {

    int start = Serial.read();
    int address, note, octave_value, command, command_value;
    
    if (start == 130) {                // 128+2 = start of transmission
      address = Serial.read();
      note = Serial.read();
      octave_value = Serial.read();
      command = Serial.read();
      command_value = Serial.read();
      sprintf(printMsg,"RX: %3d%3d%3d%3d",address,note,octave_value,command,command_value);
      printLCD(printMsg,1);
      if (address == ID || address == 128+42) { //ID or '*'
        trackerCommand(note,octave_value,command,command_value);
      }
      idle = millis();
    }
    
    if (start == 132) {
      
      byte readNoteStart, readNoteLength, readNoteSubsteps, readNoteLoops;
      byte readCommandStart, readCommandLength, readCommandSubsteps, readCommandLoops;
      
      readCommandStart = Serial.read();
      readCommandLength = Serial.read();
      readCommandSubsteps = Serial.read();
      readCommandLoops = Serial.read();
      if (readCommandStart!=255) memCommandStart = readCommandStart;
      if (readCommandLength!=255) memCommandEnd = readCommandStart+readCommandLength;
      if (readCommandSubsteps!=255) memCommandSubsteps = readCommandSubsteps;
      if (readCommandLoops!=255) memCommandLoops = readCommandLoops;

      readNoteStart = Serial.read();
      readNoteLength = Serial.read();
      readNoteSubsteps = Serial.read();
      readNoteLoops = Serial.read();
      if (readNoteStart!=255) memNoteStart = readNoteStart;
      if (readNoteLength!=255) memNoteEnd = readNoteStart+readNoteLength;
      if (readNoteSubsteps!=255) memNoteSubsteps = readNoteSubsteps;
      if (readNoteLoops!=255) memNoteLoops = readNoteLoops;
        
      looping = true;
      
      memNotePos = memNoteStart;
      memCommandPos = memCommandStart;
      
      calculateDelays();
      
      lastNote = millis()-millisPerNote;
      lastCommand = millis()-millisPerCommand;
      
      //sprintf(printMsg,"mpn: %3d ",millisPerNote);
      //printLCD(printMsg,0);      
    }
    
    if (start == 133) { //tempo commands
      bpm = Serial.read();
      delayPerBeat = Serial.read()*4;
      Serial.read(); // start memory address notes
      Serial.read(); // end memory address notes
      Serial.read(); // start memory address commands
      Serial.read(); // end memory address commands
      
      if (bpm == 0) {
        looping = false;
      } else {
        looping = true;
        calculateDelays();
        sprintf(printMsg,"dpb: %3d ",millisPerNote);
        printLCD(printMsg,0);
      }
    }
    Serial.flush();
}

void trackerCommand(int note, int octave_value, int command, int command_value) {

  if (command == KILL_NOTE) {
    if (command_value == 0) {
      gain = 0;
    } else {
      killTime = command_value + millis();
      kill = true;
    }
  }
   
  if (command == OCTAVE_REL) {
    octave += command_value-64;
    //sprintf(printMsg,"octave: %3d",octave);
    //printLCD(printMsg,0);
  }
  
  if (command == OCTAVE_ABS) {
    octave = command_value-64;
    //sprintf(printMsg,"octave: %3d",octave);
    //printLCD(printMsg,0);
  }

  if (command == GAIN_REL) {
    int result_gain = gain + (command_value-64);
    gain = constrain(result_gain,0,255);
    //sprintf(printMsg,"gain rel: %3d%3d",command_value-64,gain);
    //printLCD(printMsg,0);
  }
  
  if (command == GAIN_ABS) {
    gain = command_value*2;
    //sprintf(printMsg,"gain abs: %3d",gain);
    //printLCD(printMsg,0);
  }
  
  if (command == WAVE) {
    waveForm = command_value%numberOfWaveforms;
    //instrument = instrument%numberOfWaveforms;
  }
  
  if (command == TEMPO) {
    if (command_value == 0) command_value=1;
    bpm = map(command_value,1,255,1,1000);
    //millisPerSequenceStep = 60000/bpm/sequenceSubsteps;
    //sprintf(printMsg,"mps: %5d",millisPerSequenceStep);
    printLCD(printMsg,0);
  }
  
  if (note != 255 && octave_value != 255) {
    frequency = hertzTable[note] * pow(2,octave_value) * pow(2,octave);
    kill = false;

    gain = 255;
    lastNote = millis();
    //sprintf(printMsg,"note set: %3d %2d",note,octave);
    //printLCD(printMsg,0);
  }
  if (note != 255 && octave_value == 255) {
    frequency = hertzTable[note] * pow(2,octave);
    kill = false;
    lastNote = millis();
    //gain = 255;
    //sprintf(printMsg,"note set: %3d %2d",note,octave);
    //printLCD(printMsg,0);
  }
  
  //Serial.flush();
}

void calculateDelays() {  
  //millisPerNote = 60000/bpm/memNoteSubsteps; //how many millis pr beat
  //millisPerCommand = 60000/bpm/memCommandSubsteps;
  millisPerNote = delayPerBeat;
  millisPerCommand = delayPerBeat;
  //sprintf(printMsg,"m%15d ",millisPerNote);
  //printLCD(printMsg,0);
}


void stylophoneInput () {
  //read stylophone input
  int styloRead = analogRead(STYLOPHONE_INPUT);
  if (styloRead > styloNoise) {
    
    gain = startAttack;
    decaying = false;
    attacking = true;
    //sprintf(printMsg, "stylophone: %4d", styloRead);
    //printLCD (printMsg,0);    
    
    lastUse = millis();
    lastNote = millis();
    lastAttack = millis();
    instructionsMode = false;
    instruction = 0;
    if (styloMapping) {
      activeKey = stylophoneMapping (styloRead);

      //calculate frequency
      frequency = hertzTable[activeKey] * pow(2,octave) + pitchBend;
    } else {
      frequency = styloRead * pow(2,octave) + pitchBend;
    }
    
    if (activeKey != oldActiveKey) {
      //append string and int
      sprintf(printMsg, "stylophone: %2s%1d", noteNames[activeKey], octaveAdd[activeKey]+1);
      printLCD (printMsg,0);    
      oldActiveKey = activeKey;
    }
  } else {
    activeKey = -1;
  }  
}

int stylophoneMapping(int readVal){
  
  //Serial.print(readVal);
  //Serial.print("\t");
  
  int diff = 1023;
  int diffId = 0;

  for (int i = 0; i<20; i++) {
    if (abs(readVal - (int) pgm_read_word_near(&styloVal[i])) < diff) {
      diff = abs(readVal - (int) pgm_read_word_near(&styloVal[i]));
      diffId = i;
    }
  }
  return diffId;
}

void compileBools () {
  for (int i=0; i<16; i++) {
    if (i==counter) {
      gate[i] = true;
    } else {
      gate[i] = false;
    }
  }
}

void checkModeChange () {
  //check if we have changed mode
  if (mode != oldMode) {
    mode = mode % numberOfModes;
    printLCD(modeName[mode],0);
    oldMode = mode;
  }
}




// read buttons
// read analog input and map to corresponding button values
// buttons: down, left, ok, right, up, home
void readButtons () {
  int buttonRead = analogRead(BUTTONS);
  if (buttonRead > buttonNoise) {
    lastKeyPress = millis();
    lastUse = millis();
    instructionsMode = false;
    activeButton = buttonMapping (buttonRead);
    if (oldActiveButton != activeButton) {
      buttonPushed = true;
      oldActiveButton = activeButton;
    } else {
      buttonPushed = false;
      oldActiveButton = -1;    
    }
  }
}

void compileBytes () {
  for (int i = 0; i<8; i++) {
    bitWrite(data1, i, LED[i]);
  }
  for (int i = 8; i<16; i++) {
    bitWrite(data2, i-8, LED[i]);
  }
}  

void sendLEDData(byte dataA, byte dataB) {
  //write data
  digitalWrite(latchPin, 0);

  shiftOut(dataPin, clockPin, MSBFIRST, dataB);
  shiftOut(dataPin, clockPin, MSBFIRST, dataA);
  digitalWrite(latchPin, 1);
}

void readSerial() {
  byte val1 = Serial.read();
  byte val2 = Serial.read();
  Serial.print(val1);
  Serial.print(val2);
  data1 = val1;
  data2 = val2;
  
  for (int i=0; i<8; i++) {
    gate[i] = bitRead (data1, i);
    gate[i+8] = bitRead (data2, i);
  }
  
}

int buttonMapping(int readVal){
  int loPass = readVal-buttonNoise;
  int hiPass = readVal+buttonNoise;
  
  for (int i = 0; i<6; i++) {
    if ((loPass<buttonVal[i]) && (hiPass>buttonVal[i])) {
      return i;
      break;
    }
  }  
}

//print string to either of the 2 LCD rows
void printLCD(char *s, int y)
{
  if (printMsg == oldPrintMsg1) {
    Serial.println("same same 0");
    return;
  }

  
  for (int j=0; j<16; j++) {
    lcd.setCursor(j,y);
    if (j < strlen(s)) {
      lcd.print(s[j]);
      oldPrintMsg1[j] = s[j];
    } else {
      lcd.print(" ");
      oldPrintMsg1[j] = ' ';
    }
  }
}

void doEncoder(){
  if (digitalRead(encoder0PinA) == HIGH) {   // found a low-to-high on channel A
    if (digitalRead(encoder0PinB) == LOW) {  // check channel B to see which way
                                             // encoder is turning
      encoder0Pos ++;         // CCW
    } 
    else {
      encoder0Pos --;         // CW
    }
  }
  else                                        // found a high-to-low on channel A
  { 
    if (digitalRead(encoder0PinB) == LOW) {   // check channel B to see which way
                                              // encoder is turning  
      encoder0Pos --;          // CW
    } 
    else {
      encoder0Pos ++;          // CCW
    }
  }
}

void checkEncoder () {
  //check if encoder has been touched  
  encoderChange = 0;
  if (oldEncoder0Pos < encoder0Pos) {
    encoderChange = 1;
    oldEncoder0Pos = encoder0Pos;
  }
  if (oldEncoder0Pos > encoder0Pos) {
    encoderChange = -1;
    oldEncoder0Pos = encoder0Pos;
  }  
}

// this function will return the number of bytes currently free in RAM
// written by David A. Mellis
// based on code by Rob Faludi http://www.faludi.com
int availableMemory() {
  int size = 1024; // Use 2048 with ATmega328
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}

void createCharacters() {
  // arrays used to build custom characters
  // array to make an arrow pointing up
  byte CFO11[8] = {
	B01111,
	B10000,
	B10010,
	B10011,
	B10010,
	B10110,
	B10110,
	B10000
  };  
  byte CFO12[8] = {
	B11111,
	B10010,
	B10111,
	B10010,
	B10000,
	B01000,
	B00100,
	B00011
  };
  byte CFO21[8] = {
	B00000,
	B11111,
	B00000,
	B00000,
	B00000,
	B01111,
	B00000,
	B01111
  };
  
  byte CFO22[8] = {
	B01111,
	B00000,
	B00000,
	B11111,
	B00000,
	B00000,
	B00000,
	B11111
  };
  
  byte CFO31[8] = {
	B00000,
	B00000,
	B11110,
	B00001,
	B00101,
	B00001,
	B00001,
	B11001
  };
  
  byte CFO32[8] = {
	B11001,
	B00001,
	B00001,
	B11111,
	B00001,
	B00001,
	B00001,
	B11111
  };
  
  lcd.createChar(0, CFO11);
  lcd.createChar(1, CFO12);
  lcd.createChar(2, CFO21);
  lcd.createChar(3, CFO22);
  lcd.createChar(4, CFO31);
  lcd.createChar(5, CFO32);
}
