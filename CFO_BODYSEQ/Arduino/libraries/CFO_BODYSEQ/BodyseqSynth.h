/* 
 Synth.h - Friction Music library
 Copyright (c) 2013 Science Friction. 
 All right reserved.
 
 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your optionosc1modShape_ptr) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser Public License for more details.
 
 You should have received a copy of the GNU Lesser Public License
 along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 + author: Jakob Bak
 + contact: j.bak@ciid.dk
 */


#include <Arduino.h>
#include <spi4teensy3.h>
#include <EEPROM.h>
#include <Math.h>
#include "Sequencer.h"

#ifndef Synth_h // include guard
#define Synth_h

// Useful bit constants
#define BIT_8 256
#define BIT_12 4096
#define BIT_16 65536
#define BIT_20 1048576
#define BIT_24 16777216
#define BIT_28 268435456
#define BIT_32 4294967296
#define MAX_SAMPLE 32767
#define MIN_SAMPLE -32768

// Constants for bitvalues within the TCTRL1 register
#define TIE 2
#define TEN 1
#define TIF 1

// Constants for bitvalues for DAC output A and B 
#define DAC_A 0
#define DAC_B 1
// Constants for positions for control bits in dacSettings 
#define DAC_AB 7
#define DAC_BUF 6
#define DAC_GA 5
#define DAC_SHDN 4

// Output pin for cutoff filter on Monotron
#define CUTOFF_PIN 3

// Multiplexer Pins (CD4052)
#define MUX_A 8
#define MUX_B 7

#define LP6 0
#define HP6 1
#define BP6 2
#define LP24 6
#define HP24 4
#define BP24 5
#define MOOG 3
#define THRU 7

// SPI pins
#define MCP4251_CS 9 // Digital 9
#define DAC_CS 10  // Digital 10

#define SAMPLE_RATE 48000
#define CPU_FREQ 96 // in MHz
#define PERIOD_MAX BIT_32

// Sampler specific constants
#define NUM_SAMPLES 8

// Specify highest and lowest pitch in Hz
#define LOW_PITCH 55
#define HIGH_PITCH 1000

// Shortnames for waveforms
#define SINE 0
#define	SQUARE 1
#define PULSE 2
#define TRIANGLE 3
#define SAW 4
#define FUZZ 5
#define DIGI1 6
#define DIGI2 7
#define DIGI3 8
#define DIGI4 9
#define NOISE 10
#define DIGI6 11
#define TAN1 12
#define TAN2 13
#define TAN3 14
#define TAN4 15

#define WAVEFORM_TRIANGLE 0
#define WAVEFORM_SAW 1
#define WAVEFORM_SQUARE 2
#define WAVEFORM_ALTERNATE 3

// Maximum possible value for amplification envelope in audio code
#define MAX_ENV_GAIN 65535
#define MIN_ENV_GAIN -65535

// MIDI specific constants

#define MIDI_SERIAL Serial1
#define MIDI_THROUGH true

#ifndef MIDI_CHANNEL
	#define MIDI_CHANNEL 1
#elif (MIDI_CHANNEL > 0)&&(MIDI_CHANNEL < 17)
#else
	#error MIDI_CHANNEL should be between 1 - 16
#endif

// parameters for modulation
#define MOD_FULL 0
#define MOD_ENV1 1
#define MOD_ENV2 2
//#define MOD_ENV0 9
#define MOD_OSC1 3
#define MOD_OSC2 4
#define MOD_OSC3 5

// parameters for presets || the two parameters below should multiply to 2048.
#define MAX_PRESETS 16
#define PRESET_SIZE 128

#define BANK_U 0
#define BANK_A 16
#define BANK_B 32
#define BANK_C 48


// Constants defined for MIDI CLOCK
#define MIDI_CLOCK 0xF8     // 248
#define MIDI_START 0xFA     // 250;
#define MIDI_CONTINUE 0xFB  // 251;
#define MIDI_STOP 0xFC      // 252;

//synth functions and parameters as MIDI controller numbers
#define PRESET_SAVE 0
#define PRESET_RECALL 1

#define IS_12_BIT 3
#define CUTOFF 4
#define ZERO_HZ_FM 5
#define FM_OCTAVES 6
#define RESONANCE_OLD 7
#define PORTAMENTO 8
#define FILTER_TYPE 9

#define LFO1 10
#define SEMITONE1 11
#define DETUNE1 12
#define GAIN1 13
#define WAVEFORM1 14
#define FM1 15
#define FM1_OCTAVES 16
#define FM1_SOURCE 17
#define FM1_SHAPE 18
#define FREQUENCY1 19

#define LFO2 20
#define SEMITONE2 21
#define DETUNE2 22
#define GAIN2 23
#define WAVEFORM2 24
#define FM2 25
#define FM2_OCTAVES 26
#define FM2_SOURCE 27
#define FM2_SHAPE 28
#define FREQUENCY2 29

#define LFO3 30
#define SEMITONE3 31
#define DETUNE3 32
#define GAIN3 33
#define WAVEFORM3 34
#define FM3 35
#define FM3_OCTAVES 36
#define FM3_SOURCE 37
#define FM3_SHAPE 38
#define FREQUENCY3 39

#define SONG_PART 40
#define SONG_KEY 41
#define SONG_BPM 42 // not needed

#define CUTOFF_MOD_AMOUNT 70
#define CUTOFF_MOD_DIRECTION 71
#define CUTOFF_SOURCE 72
#define CUTOFF_SHAPE 73
#define RESONANCE_MOD_AMOUNT 74
#define RESONANCE_MOD_DIRECTION 75
#define RESONANCE_SOURCE 76
#define RESONANCE_SHAPE 77
#define CUTOFF_FREQUENCY 78
#define RESONANCE 79

#define SEQ_INTERNAL_CLOCK 80
#define SEQ_CLOCK_IN 81
#define SEQ_CLOCK_THRU 82
#define SEQ_CLOCK_OUT 83
#define SEQ_BPM 84
#define SEQ_SEQUENCE 85
#define SEQ_POSITION 86
#define SEQ_VALUE 87
#define SEQ_INTERNAL 88
#define SEQ_EXTERNAL 89
#define SEQ_STEPS 90
#define SEQ_BEGIN 91
#define SEQ_END 92
#define SEQ_SUBDIV 93
#define SEQ_LOOP 94 // NO_LOOP, LOOP [, PINGPONG, BACKWARDS, STEP]
#define SEQ_START 95
#define SEQ_STOP 96
#define SEQ_CONTINUE 97
#define SEQ_JUMP_POSITION 98

#define ENVELOPE_MULTIPLIER 100

//#define CFO_COMMAND 90
//#define CFO_LIGHT_LED 91
//#define SEQ_STEP_FORWARD 0


#define ENV0_VELOCITY 102
#define ENV0_ENABLE 103
#define ENV0_ATTACK 104
#define ENV0_DECAY 105
#define ENV0_SUSTAIN 106
#define ENV0_RELEASE 107

#define ENV1_VELOCITY 112
#define ENV1_ENABLE 113
#define ENV1_ATTACK 114
#define ENV1_DECAY 115
#define ENV1_SUSTAIN 116
#define ENV1_RELEASE 117

#define ENV2_VELOCITY 122
#define ENV2_ENABLE 123
#define ENV2_ATTACK 124
#define ENV2_DECAY 125
#define ENV2_SUSTAIN 126
#define ENV2_RELEASE 127


// MMusic class for handling sound engine

class MMusic {
    friend class MSequencer;
public:
	
	// INITIALIZER
    void init();
	void spi_setup();
	void set12bit(bool b);
	bool is12bit;
	
	// PRESETS
	void getPreset(uint8_t p);
	void getRandomizedPreset(uint8_t p, uint8_t r);
	void savePreset(uint8_t p);
	void sendInstrument();
	void loadAllPresets();

	// AUDIO INTERRUPT SERVICE ROUTINE
	void synthInterrupt8bit();
	void synthInterrupt8bitFM();
	void synthInterrupt12bitSine();
	void synthInterrupt12bitSineFM();
	void synthInterrupt12bitSawFM();
    void phaseDistortionOscillator();
    void samplerInterrupt();
    
	void envelope1();
	void envelope2();
    void envelopeRC();
	void amplifier();
	void sendToDAC(); // sending both sound and cutoff
	void output2DAC(); // sending only sound
    void output2T3DAC();    // sending sample to Teensy3.1 DAC on pin 14
    
    void setSampler(bool s);
    bool isSampler();
    void setSynth(bool s);
    bool isSynth();


    // SAMPLER FUNCTIONS
    void setBitcrush(int b);
	
	// FILTER FUNCTIONS
	void filter();
	void filterLP6dB();
	void filterHP6dB();
    void filterLP24dB();
    void filterHP24dB();
    void filterMoogLadder();
	void setCutoff(uint16_t c);
//	void setResonance(uint8_t res);
	void setResonance(uint32_t res);
    void setFilterType(uint8_t type);
	void setCutoffModAmount(int32_t amount);
	void setCutoffModDirection(int32_t direction);
	void setCutoffModSource(uint8_t source);
	void setResonanceModSource(uint8_t source);
	void setCutoffModShape(uint8_t shape);
	void setResonanceModShape(uint8_t shape);

    void generateFilterCoefficientsMoogLadder();
    
    bool lowpass;
    bool highpass;
    bool lowpass24dB;
    bool highpass24dB;
    bool moogLadder;


		
	// FREQUENCY AND DETUNE FUNCTIONS
	void setFrequency(float frequency);
	void setFrequency1(float frequency1);
	void setFrequency2(float frequency2);
	void setFrequency3(float frequency3);
	void setSemitone1(int8_t semi);
	void setSemitone2(int8_t semi);
	void setSemitone3(int8_t semi);
	void setDetune(float detune);
	void setDetune1(float detune);
	void setDetune2(float detune);
	void setDetune3(float detune);
	void setOsc1LFO(bool lfo);
	void setOsc2LFO(bool lfo);
	void setOsc3LFO(bool lfo);
	void setFM1(uint8_t fm);
	void setFM2(uint8_t fm);
	void setFM3(uint8_t fm);
	void setFMoctaves(uint8_t octs);	// THIS SHOULD PROBABLY BE CALLED SOMETHING ELSE
	void setFM1octaves(uint8_t octs);
	void setFM2octaves(uint8_t octs);
	void setFM3octaves(uint8_t octs);
	void setFM1Source(uint8_t source);
	void setFM2Source(uint8_t source);
	void setFM3Source(uint8_t source);
	void setFM1Shape(uint8_t shape);
	void setFM2Shape(uint8_t shape);
	void setFM3Shape(uint8_t shape);
	void fmToZeroHertz(bool);			// THIS SHOULD PROBABLY BE CALLED SOMETHING ELSE
	void pitchBend(float b); // NOT IMPLEMENTED
	void setPortamento(int32_t port);
	
	// WAVEFORM FUNCTIONS
	void setWaveform(uint16_t waveForm);    // JUST FOR 8bit WAVEFORMS
	void setWaveform1(uint16_t waveForm);   //
	void setWaveform2(uint16_t waveForm);   //
	void setWaveform3(uint16_t waveForm);   //
	
	// GAIN FUNCTIONS
	void setGain(float value); // 0.0 - 1.0          
	void setGain1(float value); // 0.0 - 1.0         
	void setGain2(float value); // 0.0 - 1.0         
	void setGain3(float value); // 0.0 - 1.0         
	float getGain();       // 0.0 - 1.0         
	float getGain1();       // 0.0 - 1.0         
	float getGain2();       // 0.0 - 1.0         
	float getGain3();       // 0.0 - 1.0         

	// NOTE FUNCTIONS
	void noteOn(uint8_t note, uint8_t vel); // 0 - 127
	void noteOn(uint8_t note); // 0 - 127
	void noteOff(uint8_t note); // 0 - 127
	void noteOff();
    void noteOnSample(uint8_t); // 0 - NUM_SAMPLES
	float getNoteFrequency(uint8_t note); // 0 - 127
    
	// ENVELOPE FUNCTIONS
	void enableEnvelope1();
	void disableEnvelope1();
	void setEnv1Stage(uint8_t stage1); // 0 - 4
	void setEnv1Attack(uint8_t att); // 0 - 127              
	void setEnv1Decay(uint8_t dec); // 0 - 127               
	void setEnv1Sustain(uint8_t sus); // 0 - 127             
	void setEnv1Release(uint8_t rel); // 0 - 127             
	void setEnv1VelSustain(uint8_t vel); // 0 - 127
	void setEnv1VelPeak(uint8_t vel); // 0 - 127
	void setEnvelopeMultiplier(uint8_t mult);
	
	void enableEnvelope2();
	void disableEnvelope2();
	void setEnv2Stage(uint8_t stage); // 0 - 4
	void setEnv2Attack(uint8_t att); // 0 - 127              
	void setEnv2Decay(uint8_t dec); // 0 - 127               
	void setEnv2Sustain(uint8_t sus); // 0 - 127             
	void setEnv2Release(uint8_t rel); // 0 - 127             
	void setEnv2VelSustain(uint8_t vel); // 0 - 127
	void setEnv2VelPeak(uint8_t vel); // 0 - 127
    
	bool osc1LFO;
	bool osc2LFO;
	bool osc3LFO;
    
    int32_t oscil1;
	int32_t oscil2;
	int32_t oscil3;

    // final sample that goes to the DAC
    volatile int64_t sample;
    


	
private:
	// TIMER VARIABLES
	uint32_t sampleRate;
	
	// WAVEFORM VARIABLES
	uint16_t waveForm1;
	uint16_t waveForm2;
	uint16_t waveForm3;
	uint16_t waveForm;
//	bool sine;
//	bool saw;
//	bool square;
    
	// FREQUENCY VARIABLES
	uint16_t frequency16bit;
	float frequency;
	float frequency1;
	float frequency2;
	float frequency3;
	float semi1;
	float semi2;
	float semi3;
	float detune1;
	float detune2;
	float detune3;
	float bend;
	
	// OSCILLATOR VARIABLES
	int32_t period1;
	int32_t period2;
	int32_t period3;
	int32_t portamento;
	volatile int32_t dPhase1;
	volatile int32_t dPhase2;
	volatile int32_t dPhase3;
	uint32_t accumulator1;
	uint32_t accumulator2;
	uint32_t accumulator3;
	int32_t vectorAccumulator1;
	int32_t vectorAccumulator2;
	int32_t vectorAccumulator3;
	int32_t index1;
	int32_t index2;
	int32_t index3;
	int32_t fraction1;
	int32_t fraction2;
	int32_t fraction3;
	int64_t modulator1;
	int64_t modulator2;
	int64_t modulator3;
	int32_t fullSignal;
	int32_t invertSignal;
	int32_t noSignal;
	int32_t *osc1modSource_ptr;
	int32_t *osc2modSource_ptr;
	int32_t *osc3modSource_ptr;
	int32_t *amp_modSource_ptr;
	int32_t *osc1modShape_ptr;
	int32_t *osc2modShape_ptr;
	int32_t *osc3modShape_ptr;
	int32_t *amp_modShape_ptr;
	int32_t zeroFM;
	int32_t fmAmount1;
	int32_t fmAmount2;
	int32_t fmAmount3;
	int32_t fmOctaves1;
	int32_t fmOctaves2;
	int32_t fmOctaves3;

	
	int32_t gain;
	int32_t gain1;
	int32_t gain2;
	int32_t gain3;
	
	// FILTER VARIABLES
    
    int64_t a0;
    int64_t a1;
    int64_t a2;
    int64_t a3;
    int64_t a4;
    
    int64_t b0;
    int64_t b1;
    int64_t b2;
    int64_t b3;
    int64_t b4;

    int64_t x0;
    int64_t x1;
    int64_t x2;
    int64_t x3;
    int64_t x4;
    
    int64_t y0;
    int64_t y1;
    int64_t y2;
    int64_t y3;
    int64_t y4;
    
    int64_t xNew;
    int64_t xOld;
    int64_t yNew;
    int64_t yOld;
    int64_t feedbackSample;
    
    volatile int64_t u;
    int64_t g;
    int64_t gg;
    int64_t ggg;
    int64_t G;
    int64_t Gstage;
    volatile int64_t S;
    
    volatile int64_t k;
    int64_t v1;
    int64_t v2;
    int64_t v3;
    int64_t v4;
    int64_t z1;
    int64_t z2;
    int64_t z3;
    int64_t z4;
	
    uint16_t cutoff;
	uint32_t resonance;
    
	int32_t cutoffModAmount;
	int32_t cutoffModDirection;
	int32_t *cutoffModSource_ptr;
	int32_t *resonanceModSource_ptr;
	int32_t *cutoffModShape_ptr;
	int32_t *resonanceModShape_ptr;

    
    int64_t lastSampleOutLP;
    int64_t lastSampleInLP;
    int64_t sampleOutLP;
    int64_t sampleInLP;
    int64_t lastSampleOutHP;
    int64_t lastSampleInHP;
    int64_t sampleOutHP;
    int64_t sampleInHP;

	
	// ENVELOPE VARIABLES
	bool envelopeOn1;
	int32_t env1;
	int32_t env1Stage;
	int32_t attack1;
	int32_t decay1;
	int32_t sustain1;
	int32_t release1;
	int32_t velSustain1;
	int32_t velPeak1;
    int32_t envTarget;

	bool envelopeOn2;
	int32_t env2;
	int32_t env2Stage;
	int32_t attack2;
	int32_t decay2;
	int32_t sustain2;
	int32_t release2;
	int32_t velSustain2;
	int32_t velPeak2;
	
	
	// NOTE VARIABLE
	uint8_t notePlayed;
	
	
	// the two bytes that go to the DAC over SPI for VCF and VCA
	volatile uint8_t dacSPIA0;
	volatile uint8_t dacSPIA1;
	volatile uint8_t dacSPIB0;
	volatile uint8_t dacSPIB1;
	volatile uint8_t dacSetA;
	volatile uint8_t dacSetB;

	int bitcrush;    
    bool sampler;
    bool synth;
    
};


extern MMusic Music;


// MMidi class for handling MIDI implementation

class MMidi {
    friend class MSequencer;
public:
	void init();
	void checkSerialMidi();
    
    void setChannel(uint8_t channel);
    uint8_t getChannel();
    uint8_t midiChannel;
	
	void midiHandler();
    void midiRealTimeHandler(uint8_t data);
    
	void noteOff(uint8_t channel, uint8_t note, uint8_t vel);
	void noteOn(uint8_t channel, uint8_t note, uint8_t vel);
	void aftertouch(uint8_t channel, uint8_t note, uint8_t pressure);
	void controller(uint8_t channel, uint8_t number, uint8_t value);
	void programChange(uint8_t channel, uint8_t number);
	void channelPressure(uint8_t channel, uint8_t pressure);
	void pitchWheel(uint8_t channel, uint8_t highBits, uint8_t lowBits);
	void pitchChange(uint8_t channel, int pitch); // extra pitchWheel function for USB MIDI interfacing
    void clock();
    void stop();
    void start();
    void continues();

    void sendNoteOff(uint8_t channel, uint8_t note);
    void sendNoteOff(uint8_t channel, uint8_t note, uint8_t vel);
    void sendNoteOn(uint8_t channel, uint8_t note, uint8_t vel);
    void sendController(uint8_t channel, uint8_t number, uint8_t value);
    
    void sendClock();
    void sendStart();
    void sendContinue();
    void sendStop();
    
    void setMidiIn(bool i);
    bool getMidiIn();
    
    void setMidiOut(bool o);
    bool getMidiOut();
    
    void setMidiThru(bool t);
    bool getMidiThru();
    
    void setMidiClockIn(bool i);
    bool getMidiClockIn();
    
    void setMidiClockOut(bool o);
    bool getMidiClockOut();

    void setMidiClockThru(bool t);
    bool getMidiClockThru();
    
    
private:
	
	// MIDI
	uint8_t data;
	uint8_t midiBuffer[3];
    
    bool midiIn;
    bool midiOut;
    bool midiThru;
    bool midiClockIn;
    bool midiClockOut;
    bool midiClockThru;
	
	int midiBufferIndex;
	uint16_t frequency;
	uint8_t notePlayed;
    bool midiRead;
//    int notesPlayed[16];
//    int noteIndex;
};


extern MMidi Midi;


#endif // Close guard Synth_h


