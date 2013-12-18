/* 
 Friction.h - Friction Music library
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

#ifndef Friction_h // include guard
#define Friction_h


// Useful bit constants
#define BIT_8 256
#define BIT_12 4096
#define BIT_16 65536
#define BIT_20 1048576
#define BIT_24 16777216
#define BIT_28 268435456
#define BIT_32 4294967296

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

// SPI pins
#define DAC_CS 10  // Digital 10

#define SAMPLE_RATE 20000
#define CPU_FREQ 96 // in MHz
#define PERIOD_MAX BIT_32

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

// Maximum possible value for amplification envelope in audio code
#define MAX_ENV_GAIN 65535
#define MIN_ENV_GAIN -65535

// MIDI specific constants
#ifndef MIDI_CHANNEL
	#define MIDI_CHANNEL 1
#elif (MIDI_CHANNEL > 0)&&(MIDI_CHANNEL < 17)
#else
	#error MIDI_CHANNEL should be between 1 - 16
#endif

//synth parameters as MIDI controller numbers
#define IS_12_BIT 3
#define CUTOFF 4
#define ZERO_HZ_FM 5
#define FM_OCTAVES 6
#define AMP_ENV 7
#define PORTAMENTO 8	// not implemented yet

#define FREQUENCY1 10
#define SEMITONE1 11
#define DETUNE1 12
#define GAIN1 13
#define WAVEFORM1 14
#define FM1 15
#define FM1_OCTAVES 16
#define FM1_SOURCE 17
#define FM1_SHAPE 18
#define LFO1 19

#define FREQUENCY2 20
#define SEMITONE2 21
#define DETUNE2 22
#define GAIN2 23
#define WAVEFORM2 24
#define FM2 25
#define FM2_OCTAVES 26
#define FM2_SOURCE 27
#define FM2_SHAPE 28
#define LFO2 29

#define FREQUENCY3 30
#define SEMITONE3 31
#define DETUNE3 32
#define GAIN3 33
#define WAVEFORM3 34
#define FM3 35
#define FM3_OCTAVES 36
#define FM3_SOURCE 37
#define FM3_SHAPE 38
#define LFO3 39

#define ENV1_ENABLE 113
#define ENV1_ATTACK 114
#define ENV1_DECAY 115
#define ENV1_SUSTAIN 116
#define ENV1_RELEASE 117

#define ENV2_ENABLE 123
#define ENV2_ATTACK 124
#define ENV2_DECAY 125
#define ENV2_SUSTAIN 126
#define ENV2_RELEASE 127


const uint16_t sineTable[] = { 
#include <FrictionSineTable16bitHex.inc>
};

const uint16_t waveTable[] = { 
#include <FrictionWaveTable.inc>
};

const float hertzTable[] = {
#include <FrictionHertzTable.inc>	
};

// Table of MIDI note values to frequency in Hertz
//uint16_t hertzTable[] = {8,8,9,9,10,10,11,12,12,13,14,15,16,17,18,19,20,21,23,24,25,27,29,30,32,34,36,38,41,43,46,48,51,54,58,61,65,69,73,77,82,87,92,97,103,109,116,123,130,138,146,155,164,174,184,195,207,219,233,246,261,277,293,311,329,349,369,391,415,440,466,493,523,554,587,622,659,698,739,783,830,880,932,987,1046,1108,1174,1244,1318,1396,1479,1567,1661,1760,1864,1975,2093,2217,2349,2489,2637,2793,2959,3135,3322,3520,3729,3951,4186,4434,4698,4978,5274,5587,5919,6271,6644,7040,7458,7902,8372,8869,9397,9956,10548,11175,11839,12543};

// Used in the functions that set the envelope timing
//uint32_t envTimeTable[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,73,75,77,79,81,83,85,87,89,92,95,98,101,104,108,112,116,120,124,128,132,136,140,145,150,155,160,165,170,176,182,188,194,201,208,215,222,230,238,247,257,268,280,293,307,322,341,379,425,482,556,654,792,998,1342,2030,4095};
//uint32_t envTimeTable[] = {1,5,9,13,17,21,26,30,35,39,44,49,54,59,64,69,74,79,85,90,96,101,107,113,119,125,132,138,144,151,158,165,172,179,187,194,202,210,218,226,234,243,252,261,270,279,289,299,309,320,330,341,353,364,376,388,401,414,427,440,455,469,484,500,516,532,549,566,584,602,622,642,663,684,706,729,753,778,804,831,859,888,919,951,984,1019,1056,1094,1134,1176,1221,1268,1317,1370,1425,1484,1547,1614,1684,1760,1841,1929,2023,2125,2234,2354,2484,2627,2785,2959,3152,3368,3611,3886,4201,4563,4987,5487,6087,6821,7739,8918,10491,12693,15996,21500,32509,65535};
const uint32_t envTimeTable[] = {1,5,9,14,19,26,34,42,53,65,79,95,113,134,157,182,211,243,278,317,359,405,456,511,570,633,702,776,854,939,1029,1124,1226,1333,1448,1568,1695,1829,1971,2119,2274,2438,2610,2789,2977,3172,3377,3590,3813,4044,4285,4535,4795,5065,5345,5635,5936,6247,6569,6902,7247,7602,7970,8349,8740,9143,9559,9986,10427,10880,11347,11827,12321,12828,13349,13883,14433,14996,15574,16167,16775,17398,18036,18690,19359,20045,20746,21464,22198,22949,23716,24501,25303,26122,26959,27813,28686,29577,30486,31413,32359,33325,34309,35312,36335,37378,38440,39522,40625,41748,42892,44056,45241,46448,47675,48925,50196,51489,52803,54141,55500,56883,58288,59716,61167,62642,64140,65662};

const float semitoneTable[] = {0.25,0.2648658,0.2806155,0.29730177,0.31498027,0.33370996,0.35355338,0.37457678,0.39685026,0.4204482,0.44544938,0.47193715,0.5,0.5297315,0.561231,0.59460354,0.62996054,0.6674199,0.70710677,0.74915355,0.7937005,0.8408964,0.8908987,0.9438743,1.0,1.0594631,1.122462,1.1892071,1.2599211,1.3348398,1.4142135,1.4983071,1.587401,1.6817929,1.7817974,1.8877486,2.0,2.1189263,2.244924,2.3784142,2.5198421,2.6696796,2.828427,2.9966142,3.174802,3.3635857,3.563595,3.7754972,4.0};

const uint32_t portamentoTimeTable[] = {1,5,9,13,17,21,26,30,35,39,44,49,54,59,64,69,74,79,85,90,96,101,107,113,119,125,132,138,144,151,158,165,172,179,187,194,202,210,218,226,234,243,252,261,270,279,289,299,309,320,330,341,353,364,376,388,401,414,427,440,455,469,484,500,516,532,549,566,584,602,622,642,663,684,706,729,753,778,804,831,859,888,919,951,984,1019,1056,1094,1134,1176,1221,1268,1317,1370,1425,1484,1547,1614,1684,1760,1841,1929,2023,2125,2234,2354,2484,2627,2785,2959,3152,3368,3611,3886,4201,4563,4987,5487,6087,6821,7739,8918,10491,12693,15996,21500,32509,65535};


// MMusic class for handling sound engine

class MMusic {    
public:
	
	// INITIALIZER
    void init();
	void spi_setup();
	void timer_setup();
	void synthInterrupt();
	void set12bit(bool b);
	bool is12bit;


	// AUDIO INTERRUPT SERVICE ROUTINE
	void synthInterrupt8bit();
	void synthInterrupt8bitFM();
	void synthInterrupt12bitSine();
	void synthInterrupt12bitSineFM();
	void envelope1();
	void envelope2();
	void amplifier();
	void sendToDAC(); // sending both sound and cutoff
	void sendSampleToDAC(); // sending only sound
	
	// FILTER FUNCTIONS
	void filter();
	void setCutoff(int32_t c);
	void setResonance(int32_t res);
	
	// MONOTRON FILTER MOD
	void monotronFilter();

		
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
	void setFMoctaves(uint8_t octs);
	void setFM1octaves(uint8_t octs);
	void setFM2octaves(uint8_t octs);
	void setFM3octaves(uint8_t octs);
	void setFM1Source(uint8_t source);
	void setFM2Source(uint8_t source);
	void setFM3Source(uint8_t source);
	void setFM1Shape(uint8_t shape);
	void setFM2Shape(uint8_t shape);
	void setFM3Shape(uint8_t shape);
	void fmToZeroHertz(bool);
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
	
private:
	// TIMER VARIABLES
	uint32_t sampleRate;
	
	// WAVEFORM VARIABLES
	uint16_t waveForm1;
	uint16_t waveForm2;
	uint16_t waveForm3;
	uint16_t waveForm;
	bool sine;
	bool saw;
	bool square;
	
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
	int32_t index1;
	int32_t index2;
	int32_t index3;
	int32_t fraction1;
	int32_t fraction2;
	int32_t fraction3;
	int32_t oscil1;
	int32_t oscil2;
	int32_t oscil3;
//	uint32_t oscilLast1;
//	uint32_t oscilLast2;
//	uint32_t oscilLast3;
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
	int32_t cutoff;
	int32_t resonance;
	
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
	
	// final sample that goes to the DAC    
	int64_t sample;
	
	// the two bytes that go to the DAC over SPI
	volatile uint8_t dacSPI0;
	volatile uint8_t dacSPI1;
	// the two bytes that go to the DAC over SPI for stereo -  NOT IMPLEMENTED
	volatile uint8_t dacSPIA0;
	volatile uint8_t dacSPIA1;
	volatile uint8_t dacSPIB0;
	volatile uint8_t dacSPIB1;
	volatile uint8_t dacSetA;
	volatile uint8_t dacSetB;
    
};

extern MMusic Music;

IntervalTimer synthTimer;




#ifdef MIDI

// MMidi class for handling MIDI implementation

class MMidi {
public:
	void init();
	void checkMidi();
	
	void midiHandler();
	void noteOff(uint8_t channel, uint8_t note, uint8_t vel);
	void noteOn(uint8_t channel, uint8_t note, uint8_t vel);
	void aftertouch(uint8_t channel, uint8_t note, uint8_t pressure);
	void controller(uint8_t channel, uint8_t number, uint8_t value);
	void programChange(uint8_t channel, uint8_t number);
	void channelPressure(uint8_t channel, uint8_t pressure);
	void pitchWheel(uint8_t channel, uint8_t highBits, uint8_t lowBits);
	void pitchChange(uint8_t channel, int pitch); // extra pitchWheel function for USB MIDI interfacing
	
private:
	
	// MIDI
	uint8_t data;
	uint8_t midiBuffer[3];
	uint8_t midiChannel;
	
	int midiBufferIndex;
	uint16_t frequency;
	uint8_t notePlayed;
};

extern MMidi Midi;

#endif



//////////////////////////////////////////////////////////
//
// SYNTH INTERRUPT - The pre-processor selects 8 or 12 bit
//
//////////////////////////////////////////////////////////

void synth_isr(void) {
	
	Music.sendSampleToDAC();
//	Music.sendToDAC();
	
	Music.envelope1();
	Music.envelope2();
	
	if(Music.is12bit) Music.synthInterrupt12bitSineFM();
	else Music.synthInterrupt8bitFM();
		
	Music.amplifier();

	//	Music.monotronFilter();
	Music.filter();



}


void MMusic::set12bit(bool b) {
	is12bit = b;
}
	



/////////////////////////////////////////////////////////
//
//	8 BIT OSCILLATOR - WAVETABLE - 16 WAVEFORMS
//
/////////////////////////////////////////////////////////



void MMusic::synthInterrupt8bitFM ()
{
	
	dPhase1 = dPhase1 + (period1 - dPhase1) / portamento;
//	modulator1 = (fmAmount1 * fmOctaves1 * (oscil3-15384))>>5;
	modulator1 = (fmAmount1 * fmOctaves1 * (*osc1modSource_ptr))>>10;
//	modulator1 = (fmAmount1 * (*osc1modSource_ptr))>>10;
	modulator1 = (modulator1 * (*osc1modShape_ptr))>>16;
	modulator1 = (modulator1 * int64_t(dPhase1))>>16;
	modulator1 = (modulator1>>((modulator1>>31)&zeroFM));
	accumulator1 = accumulator1 + dPhase1 + modulator1;
	index1 = accumulator1 >> 24;
	oscil1 = waveTable[index1 + waveForm1];
	oscil1 -= 128;
	oscil1 <<= 8;
	sample = (oscil1 * gain1);
	
	dPhase2 = dPhase2 + (period2 - dPhase2) / portamento;
	modulator2 = (fmAmount2 * fmOctaves2 * (*osc2modSource_ptr))>>10;
//	modulator2 = (fmAmount2 * (*osc2modSource_ptr))>>10;
	modulator2 = (modulator2 * (*osc2modShape_ptr))>>16;
	modulator2 = (modulator2 * int64_t(dPhase2))>>16;
	modulator2 = (modulator2>>((modulator2>>31)&zeroFM));
	accumulator2 = accumulator2 + dPhase2 + modulator2;
	index2 = accumulator2 >> 24;
	oscil2 = waveTable[index2 + waveForm2];
	oscil2 -= 128;
	oscil2 <<= 8;
	sample += (oscil2 * gain2);

	dPhase3 = dPhase3 + (period3 - dPhase3) / portamento;
	modulator3 = (fmAmount3 * fmOctaves3 * (*osc3modSource_ptr))>>10;
//	modulator3 = (fmAmount3 * (*osc3modSource_ptr))>>10;
	modulator3 = (modulator3 * (*osc3modShape_ptr))>>16;
	modulator3 = (modulator3 * int64_t(dPhase3))>>16;
	modulator3 = (modulator3>>((modulator3>>31)&zeroFM));
	accumulator3 = accumulator3 + dPhase3 + modulator3;
	index3 = accumulator3 >> 24;
	oscil3 = waveTable[index3 + waveForm3];
	oscil3 -= 128;
	oscil3 <<= 8;
	sample += (oscil3 * gain3);
	
	sample >>= 18;
	sample += 32768;

}





/////////////////////////////////////////////////////////
//
//	12 BIT OSCILLATOR - SINETABLE
//
/////////////////////////////////////////////////////////


void MMusic::synthInterrupt12bitSineFM()
{
	/*
	 dPhase1 = dPhase1 + (period1 - dPhase1) / portamento;
	 //	modulator1 = (fmAmount1 * fmOctaves1 * (oscil3-32768))>>6;
	 modulator1 = (fmAmount1 * fmOctaves1 * (*osc1modSource_ptr))>>6;
	 modulator1 = (modulator1 * (*osc1modShape_ptr))>>16;
	 modulator1 = (modulator1 * int64_t(dPhase1))>>16;
	 modulator1 = (modulator1>>((modulator1>>31)&zeroFM));
	 accumulator1 = accumulator1 + dPhase1 + modulator1;						// accumulator is 32bit
	 //	fraction1 = accumulator1;
	 //	fraction1 &= 0x000FFFFF;
	 //	fraction1 >>= 12;														// fraction is	 8bit
	 index1 = accumulator1 >> 20;											// index is		12bit
	 //	fraction1 = accumulator1 - (index1 << 20);								// fraction is	20bit
	 //	fraction1 >>= 12;														// fraction is   8bit
	 //	oscil1 = sineTable[index1] * (255 - fraction1);							// oscil is		16bit
	 //	index1++;
	 //	oscil1 *= fraction1;													// oscil is now 24bit
	 //	oscil1 += sineTable[index1] * fraction1; 
	 //	oscil1 >>= 8;															// oscil is now 16bit
	 oscil1 = sineTable[index1];
	 //	oscil1 -= 2048;
	 //	oscil1 <<= 4;
	 sample = (oscil1 * gain1);	
	 */	
	
	dPhase1 = dPhase1 + (period1 - dPhase1) / portamento;
//	modulator1 = (fmAmount1 * fmOctaves1 * (oscil3-32768))>>6;
	modulator1 = (fmAmount1 * fmOctaves1 * (*osc1modSource_ptr))>>10;
//	modulator1 = (fmAmount1 * (*osc1modSource_ptr))>>10;
	modulator1 = (modulator1 * (*osc1modShape_ptr))>>16;
	modulator1 = (modulator1 * int64_t(dPhase1))>>16;
	modulator1 = (modulator1>>((modulator1>>31)&zeroFM));
	accumulator1 = accumulator1 + dPhase1 + modulator1;
	index1 = accumulator1 >> 20;
	oscil1 = sineTable[index1];
//	oscil1 -= 2048;
	oscil1 -= 32768;
//	oscil1 <<= 4;
	sample = (oscil1 * gain1);
	
	dPhase2 = dPhase2 + (period2 - dPhase2) / portamento;
//	modulator2 = (fmAmount2 * fmOctaves2 * (oscil1-32768))>>6;
	modulator2 = (fmAmount2 * fmOctaves2 * (*osc2modSource_ptr))>>10;
//	modulator2 = (fmAmount2 * (*osc2modSource_ptr))>>10;
	modulator2 = (modulator2 * (*osc2modShape_ptr))>>16;
	modulator2 = (modulator2 * int64_t(dPhase2))>>16;
	modulator2 = (modulator2>>((modulator2>>31)&zeroFM));
	accumulator2 = accumulator2 + dPhase2+ modulator2;
	index2 = accumulator2 >> 20;
	oscil2 = sineTable[index2];
//	oscil2 -= 2048;
	oscil2 -= 32768;
//	oscil2 <<= 4;
	sample += (oscil2 * gain2); 
	
	dPhase3 = dPhase3 + (period3 - dPhase3) / portamento;
//	modulator3 = (fmAmount3 * fmOctaves3 * (oscil2-32768))>>6;
	modulator3 = (fmAmount3 * fmOctaves3 * (*osc3modSource_ptr))>>10;
//	modulator3 = (fmAmount3 * (*osc3modSource_ptr))>>10;
	modulator3 = (modulator3 * (*osc3modShape_ptr))>>16;
	modulator3 = (modulator3 * int64_t(dPhase3))>>16;
	modulator3 = (modulator3>>((modulator3>>31)&zeroFM));
	accumulator3 = accumulator3 + dPhase3 + modulator3;
	index3 = accumulator3 >> 20;
	oscil3 = sineTable[index3];
//	oscil3 -= 2048;
	oscil3 -= 32768;
//	oscil3 <<= 4;
	sample += (oscil3 * gain3);
	
	sample >>= 18;
	sample += 32768;
 	
}



/////////////////////////////////////////////////////////
//
//	ENVELOPES
//
/////////////////////////////////////////////////////////


void MMusic::envelope1() {
	
	if(envelopeOn1) {

		// Attack
		if(env1Stage == 1) {
			env1 += 1; // to make sure the envelope increases when (MAX_ENV_GAIN-env1) is smaller than attack1
			env1 += (MAX_ENV_GAIN - env1)/attack1;
			if(velPeak1 < env1) {
				env1 = velPeak1;
				env1Stage = 2;
			}
		}
		// Decay
		else if(env1Stage == 2) {
			env1 += -1;	// to make sure the envelope decreases when (velSustain1-env1) is smaller than decay1
			env1 += (velSustain1-env1)/decay1;
			if(env1 < velSustain1 || MAX_ENV_GAIN < env1) {
				env1 = velSustain1;
				env1Stage = 3;
			}
		}
		// Sustain
		else if (env1Stage == 3) {
			env1 = velSustain1;
		}

		// Release
		else if (env1Stage == 4) {
			env1 += -1; // to make sure the envelope decreases when (0-env1) is smaller than release1
			env1 += (0 - env1) / release1;
			if(env1 < 0 || MAX_ENV_GAIN < env1) {
				env1 = 0;
				env1Stage = 0;
			}
		}
				 
		// No gain
		else if (env1Stage == 0) {
			env1 = 0;
		}
				 
	} else {
		env1 = 65535;
	}

}



void MMusic::envelope2() {
	
	if(envelopeOn2) {

		// Attack
		if(env2Stage == 1) {
			env2 += 1; // to make sure the envelope increases when (MAX_ENV_GAIN-env2) is smaller than attack1
			env2 += (MAX_ENV_GAIN-env2)/attack2;
			if(velPeak2 < env2) {
				env2 = velPeak2;
				env2Stage = 2;
			}
		}
		// Decay
		else if(env2Stage == 2) {
			env2 += -1;	// to make sure the envelope decreases when (velSustain2-env2) is smaller than decay2
			env2 += (velSustain2-env2)/decay2;
			if(env2 < velSustain2 || MAX_ENV_GAIN < env2) {
				env2 = velSustain2;
				env2Stage = 3;
			}
		}
		// Sustain
		else if (env2Stage == 3) {
			env2 = velSustain2;
		}

		// Release
		else if (env2Stage == 4) {
			env2 += -1; // to make sure the envelope decreases when (0-env2) is smaller than release2
			env2 += (0 - env2) / release2;
			if(env2 < 0 || MAX_ENV_GAIN < env2) {
				env2 = 0;
				env2Stage = 0;
			}
		}

		// No gain
		else if (env2Stage == 0) {
			env2 = 0;
			//accumulator1 = 0;
			//accumulator2 = 0;
			//accumulator3 = 0;
		}

	} else {
		env2 = 65535;
	}

}


void MMusic::amplifier() {
	
	sample = (env1 * sample) >> 16;

}


/////////////////////////////////////////////////////////
//
//	SEND SAMPLE TO DAC
//
/////////////////////////////////////////////////////////


void MMusic::sendSampleToDAC() {
	
	// Formatting the samples to be transfered to the MCP4921 DAC to output A
	dacSPIA0 = sample >> 8;
	dacSPIA0 >>= 4;
	dacSPIA0 |= dacSetA; 
	dacSPIA1 = sample >> 4;

	digitalWrite(DAC_CS, LOW);
	while(SPI.transfer(dacSPIA0));
	while(SPI.transfer(dacSPIA1));
	digitalWrite(DAC_CS, HIGH);

}


void MMusic::sendToDAC() {
	
	// Formatting the samples to be transfered to the MCP4822 DAC to output A
	dacSPIA0 = sample >> 8;
	dacSPIA0 >>= 4;
	dacSPIA0 |= dacSetA; 
	dacSPIA1 = sample >> 4;
	
	digitalWrite(DAC_CS, LOW);
	while(SPI.transfer(dacSPIA0));
	while(SPI.transfer(dacSPIA1));
	digitalWrite(DAC_CS, HIGH);

	// Formatting the samples to be transfered to the MCP4822 DAC to output B
	dacSPIB0 = cutoff >> 8;
	dacSPIB0 >>= 4;
	dacSPIB0 |= dacSetB; 
	dacSPIB1 = cutoff >> 4;
	
	digitalWrite(DAC_CS, LOW);
	while(SPI.transfer(dacSPIB0));
	while(SPI.transfer(dacSPIB1));
	digitalWrite(DAC_CS, HIGH);
	
	
}



MMusic Music;

#ifdef MIDI
MMidi Midi;
#endif




/////////////////////////////////////
//
//	INITIALIZING FUNCTION
//
/////////////////////////////////////

void MMusic::timer_setup()
{	
	// Teensy 3.0 version
	SIM_SCGC6 |= SIM_SCGC6_PIT; // Activates the clock for PIT
	// Turn on PIT
	PIT_MCR = 0x00;
	// Set the period of the timer.  Unit is (1 / 50MHz) = 20ns
	// So 20 kHz frequency -> 50 µs period -> 2500 * 20ns
	PIT_LDVAL1 = (CPU_FREQ * 1000000) / SAMPLE_RATE;
	// Enable interrupts on timer1
	PIT_TCTRL1 = TIE;
	// Start the timer
	PIT_TCTRL1 |= TEN;
	NVIC_ENABLE_IRQ(IRQ_PIT_CH1); // Another step to enable PIT channel 1 interrupts
}


void MMusic::spi_setup()
{
	pinMode(DAC_CS, OUTPUT);
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV16);  // T3_Beta7 requires this, no default?	
}


void MMusic::init()
{
	sampleRate = SAMPLE_RATE;
	sample = 0;
	is12bit = false;
	setPortamento(0);
	
	dPhase1 = 0;
	dPhase2 = 0;
	dPhase3 = 0;
	modulator1 = 0;
	modulator2 = 0;
	modulator3 = 0;
	fullSignal = 65535;
	invertSignal = -65535;
	noSignal = 0;
	
	osc1modSource_ptr = &oscil2;
	osc2modSource_ptr = &oscil3;
	osc3modSource_ptr = &env1;
	amp_modSource_ptr = &env1;
	osc1modShape_ptr = &env2;
	osc2modShape_ptr = &fullSignal;
	osc3modShape_ptr = &fullSignal;
	amp_modShape_ptr = &fullSignal;

	setFM1Source(3);
	setFM2Source(1);
	setFM3Source(2);
	setFM1Shape(0);
	setFM2Shape(0);
	setFM3Shape(0);
	
	zeroFM = 1;
	accumulator1 = 0;
	accumulator2 = 0;
	accumulator3 = 0;
	index1 = 0;
	index2 = 0;
	index3 = 0;
	oscil1 = 0;
	oscil2 = 0;
	oscil3 = 0;
	
	
	// waveform setup
	setWaveform(0);
	
	// frequency setup
	setFrequency(440);
	setSemitone1(0);
	setSemitone2(0);
	setSemitone3(0);
	setDetune(0);
	setOsc1LFO(false);
	setOsc2LFO(false);
	setOsc3LFO(false);
	
	// gain setup
	setGain(1.0f);
	setGain1(1.0f);
	setGain2(1.0f);
	setGain3(1.0f);
	
	// envelope setup
	setEnv1Stage(0);
	disableEnvelope1();
	env1 = 0;

	setEnv1Attack(4);
	setEnv1Decay(90);
	setEnv1Sustain(32);
	setEnv1Release(64);
	setEnv1VelSustain(0);
	
	setEnv2Stage(0);
	disableEnvelope2();
	env2 = 0;
	
	setEnv2Attack(8);
	setEnv2Decay(36);
	setEnv2Sustain(0);
	setEnv2Release(64);
	setEnv2VelSustain(0);
	
	
	
	//FM setup
	setFM1(0);
	setFM2(0);
	setFM3(0);
	setFMoctaves(0);
	
	// filter setup
	setCutoff(4095);
	setResonance(0);
		
	// DAC setup
	dacSetA = 0;
	dacSetB = 0;
	dacSetA |= (DAC_A << DAC_AB) | (0 << DAC_BUF) | (1 << DAC_GA) | (1 << DAC_SHDN);
	dacSetB |= (DAC_B << DAC_AB) | (0 << DAC_BUF) | (1 << DAC_GA) | (1 << DAC_SHDN);
	
	spi_setup();
	cli();
	// set PWM for pin that goes to the monotron's cutoff
//	analogWriteFrequency(CUTOFF_PIN, 44100);
//	analogWriteResolution(10);
	
	synthTimer.begin(synth_isr, 1000000.0 / sampleRate);
	sei();
	
}




/////////////////////////////////////
//
//	FILTER FUNCTIONS
//
/////////////////////////////////////

void MMusic::setCutoff(int32_t c)
{
	cutoff = c;
}


void MMusic::setResonance(int32_t res)
{
	resonance = res;
}


void MMusic::monotronFilter() {
	uint32_t cutoffValue = env2 >> 6;
	analogWrite(CUTOFF_PIN, cutoffValue);
}


void MMusic::filter() {
	
	
//	uint32_t c = (env2 * ((cutoff + 65536) >> 1)) >> 16;
	int64_t c = ((((env2 * cutoff) >> 15) + 65536) >> 1);

//	uint32_t c = cutoff;
	
	// Formatting the samples to be transfered to the MCP4822 DAC to output B
	dacSPIB0 = uint32_t(c) >> 8;
	dacSPIB0 >>= 4;
	dacSPIB0 |= dacSetB; 
	dacSPIB1 = c >> 4;
	
	digitalWrite(DAC_CS, LOW);
	while(SPI.transfer(dacSPIB0));
	while(SPI.transfer(dacSPIB1));
	digitalWrite(DAC_CS, HIGH);	
	
}




/////////////////////////////////////
//
//	FREQUENCY AND DETUNE FUNCTIONS
//
/////////////////////////////////////

void MMusic::setFrequency(float freq)
{
	frequency = freq;
	if(!osc1LFO) setFrequency1(freq);
	if(!osc2LFO) setFrequency2(freq);
	if(!osc3LFO) setFrequency3(freq);
	//	period1 = int32_t(((freq * semi1 * (1 + detune1 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
	//	period2 = int32_t(((freq * semi2 * (1 + detune2 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
	//	period3 = int32_t(((freq * semi3 * (1 + detune3 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
	//	frequency1 = freq;
	//	frequency2 = freq;
	//	frequency3 = freq;
}


void inline MMusic::setFrequency1(float freq)
{
	frequency1 = freq;
	period1 = int32_t(((frequency1 * semi1 * (1 + detune1 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void inline MMusic::setFrequency2(float freq)
{
	frequency2 = freq;
	period2 = int32_t(((frequency2 * semi2 * (1 + detune2 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void inline MMusic::setFrequency3(float freq)
{
	frequency3 = freq;
	period3 = int32_t(((frequency3 * semi3 * (1 + detune3 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::setSemitone1(int8_t semi)
{
	if(-25 < semi && semi < 25){
		semi1 = semitoneTable[semi+24];
	} else if (semi < -24) {
		semi1 = semitoneTable[0];
	} else {
		semi1 = semitoneTable[48];
	}
	setFrequency1(frequency1);
	//	period1 = int32_t(((frequency1 * semi1 * (1 + detune1 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::setSemitone2(int8_t semi)
{
	if(-25 < semi && semi < 25){
		semi2 = semitoneTable[semi+24];
	} else if (semi < -24) {
		semi2 = semitoneTable[0];
	} else {
		semi2 = semitoneTable[48];
	}
	setFrequency2(frequency2);
//	period2 = int32_t(((frequency2 * semi2 * (1 + detune2 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::setSemitone3(int8_t semi)
{
	if(-25 < semi && semi < 25){
		semi3 = semitoneTable[semi+24];
	} else if (semi < -24) {
		semi3 = semitoneTable[0];
	} else {
		semi3 = semitoneTable[48];
	}
	setFrequency3(frequency3);
//	period3 = int32_t(((frequency3 * semi3 * (1 + detune3 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::setDetune(float detune)
{
	detune1 = 0.0;
	detune2 = detune*0.123456789;
	detune3 = -detune;
	setFrequency2(frequency2);
	setFrequency3(frequency3);
//	period2 = int32_t(((frequency2 * semi2 * (1 + detune2 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
//	period3 = int32_t(((frequency3 * semi3 * (1 + detune3 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::setDetune1(float detune)
{
	detune1 = detune;
	setFrequency1(frequency1);
//	period1 = int32_t(((frequency1 * semi1 * (1 + detune1 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::setDetune2(float detune)
{
	detune2 = detune;
	setFrequency2(frequency2);
//	period2 = int32_t(((frequency2 * semi2 * (1 + detune2 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::setDetune3(float detune)
{
	detune3 = detune;
	setFrequency3(frequency3);
//	period3 = int32_t(((frequency3 * semi3 * (1 + detune3 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
}


void MMusic::pitchBend(float b)
{
	bend = b;
	setFrequency1(frequency1);
	setFrequency2(frequency2);
	setFrequency3(frequency3);
//	period1 = int32_t(((frequency1 * semi1 * (1 + detune1 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
//	period2 = int32_t(((frequency2 * semi2 * (1 + detune2 + bend)) * PERIOD_MAX) / SAMPLE_RATE);
//	period3 = int32_t(((frequency3 * semi3 * (1 + detune3 + bend)) * PERIOD_MAX) / SAMPLE_RATE);	
}


void MMusic::setOsc1LFO(bool lfo) {
	osc1LFO = lfo;
}


void MMusic::setOsc2LFO(bool lfo) {
	osc2LFO = lfo;
}


void MMusic::setOsc3LFO(bool lfo) {
	osc3LFO = lfo;
}


void MMusic::setFM1(uint8_t fm) {
	fmAmount1 = fm;
}


void MMusic::setFM2(uint8_t fm) {
	fmAmount2 = fm;
//	fmAmount2 = (fm * fmOctaves2);
}


void MMusic::setFM3(uint8_t fm) {
	fmAmount3 = fm;
//	fmAmount3 = (fm * fmOctaves3);
}


void MMusic::setFMoctaves(uint8_t octs) {
//	fmAmount1 = (fmAmount1 * octs);
//	fmAmount2 = (fmAmount2 * octs);
//	fmAmount3 = (fmAmount3 * octs);
	fmOctaves1 = octs;
	fmOctaves2 = octs;
	fmOctaves3 = octs;
}


void MMusic::setFM1octaves(uint8_t octs) {
	fmOctaves1 = octs;
}


void MMusic::setFM2octaves(uint8_t octs) {
	fmOctaves2 = octs;
}


void MMusic::setFM3octaves(uint8_t octs) {
	fmOctaves3 = octs;
}


void MMusic::setFM1Source(uint8_t source) {
	switch(source) {
		case 0:
			osc1modSource_ptr = &fullSignal;
			break;
		case 1:
			osc1modSource_ptr = &oscil1;
			break;
		case 2:
			osc1modSource_ptr = &oscil2;
			break;
		case 3:
			osc1modSource_ptr = &oscil3;
			break;
		default:
			osc1modSource_ptr = &fullSignal;
			break;
	}
}
	

void MMusic::setFM2Source(uint8_t source) {
	switch(source) {
		case 0:
			osc2modSource_ptr = &fullSignal;
			break;
		case 1:
			osc2modSource_ptr = &oscil1;
			break;
		case 2:
			osc2modSource_ptr = &oscil2;
			break;
		case 3:
			osc2modSource_ptr = &oscil3;
			break;
		default:
			osc1modSource_ptr = &fullSignal;
			break;			
	}
}


void MMusic::setFM3Source(uint8_t source) {
	switch(source) {
		case 0:
			osc3modSource_ptr = &fullSignal;
			break;
		case 1:
			osc3modSource_ptr = &oscil1;
			break;
		case 2:
			osc3modSource_ptr = &oscil2;
			break;
		case 3:
			osc3modSource_ptr = &oscil3;
			break;
		default:
			osc1modSource_ptr = &fullSignal;
			break;			
	}
}


void MMusic::setFM1Shape(uint8_t shape) {
	switch(shape) {
		case 0:
			osc1modShape_ptr = &fullSignal;
			break;
		case 1:
			osc1modShape_ptr = &env1;
			break;
		case 2:
			osc1modShape_ptr = &env2;
			break;
		default:
			osc1modShape_ptr = &fullSignal;
			break;
	}
}


void MMusic::setFM2Shape(uint8_t shape) {
	switch(shape) {
		case 0:
			osc2modShape_ptr = &fullSignal;
			break;
		case 1:
			osc2modShape_ptr = &env1;
			break;
		case 2:
			osc2modShape_ptr = &env2;
			break;
		default:
			osc2modShape_ptr = &fullSignal;
			break;
	}
}


void MMusic::setFM3Shape(uint8_t shape) {
	switch(shape) {
		case 0:
			osc3modShape_ptr = &fullSignal;
			break;
		case 1:
			osc3modShape_ptr = &env1;
			break;
		case 2:
			osc3modShape_ptr = &env2;
			break;
		default:
			osc3modShape_ptr = &fullSignal;
			break;
	}
}


void MMusic::fmToZeroHertz(bool zero) {
	if(!zero) zeroFM = 1;
	else zeroFM = 0;
}

void MMusic::setPortamento(int32_t port) {
	if(port == 0) port = 1;
	portamento = port;
//	portamento = envTimeTable[uint8_t(port)];
}



/////////////////////////////////////
//
//	WAVEFORM FUNCTIONS
//
/////////////////////////////////////


void MMusic::setWaveform(uint16_t waveForm)
{
	waveForm1 = waveForm * 256;
	waveForm2 = waveForm * 256;
	waveForm3 = waveForm * 256;
}


void MMusic::setWaveform1(uint16_t waveForm)
{
	waveForm1 = waveForm * 256;
}


void MMusic::setWaveform2(uint16_t waveForm)
{
	waveForm2 = waveForm * 256;
}


void MMusic::setWaveform3(uint16_t waveForm)
{
	waveForm3 = waveForm * 256;
}




/////////////////////////////////////
//
//	GAIN FUNCTIONS
//
/////////////////////////////////////


void MMusic::setGain(float value)
{
	gain = uint16_t(value * 65535);
	gain1 = gain;
	gain2 = gain;
	gain3 = gain;
}


void MMusic::setGain1(float value)
{
	gain1 = uint16_t(value * 65535);
}


void MMusic::setGain2(float value)
{
	gain2 = uint16_t(value * 65535);
}


void MMusic::setGain3(float value)
{
	gain3 = uint16_t(value * 65535);
}


float MMusic::getGain()
{
	return float(gain)/65535.0;
}


float MMusic::getGain1()
{
	return float(gain1)/65535.0;
}


float MMusic::getGain2()
{
	return float(gain2)/65535.0;
}


float MMusic::getGain3()
{
	return float(gain3)/65535.0;
}




/////////////////////////////////////
//
//	NOTE_ON/OFF FUNCTIONS
//
/////////////////////////////////////


void MMusic::noteOn(uint8_t note, uint8_t vel)
{	
	env1Stage = 1;
	env2Stage = 1;
	setEnv1VelSustain(vel);
	setEnv2VelSustain(vel);
	setEnv1VelPeak(vel);
	setEnv2VelPeak(vel);
	notePlayed = note;
	frequency16bit = hertzTable[notePlayed];
	setFrequency(frequency16bit);
	//setFrequency1(frequency16bit);
	//setFrequency2(frequency16bit);
	//setFrequency3(frequency16bit);
}


void MMusic::noteOn(uint8_t note)
{	
	int vel = 127;
	env1Stage = 1;
	env2Stage = 1;
	setEnv1VelSustain(vel);
	setEnv2VelSustain(vel);
	setEnv1VelPeak(vel);
	setEnv2VelPeak(vel);
	notePlayed = note;
	frequency16bit = hertzTable[notePlayed];
	setFrequency(frequency16bit);
	//setFrequency1(frequency16bit);
	//setFrequency2(frequency16bit);
	//setFrequency3(frequency16bit);
}


void MMusic::noteOff(uint8_t note)
{	
	if(notePlayed == note) {
		env1Stage = 4;
		env2Stage = 4;
	}    
}


void MMusic::noteOff()
{	
	env1Stage = 4;
	env2Stage = 4;
}


float MMusic::getNoteFrequency(uint8_t note)
{
	return hertzTable[note];
}




/////////////////////////////////////
//
//	ENVELOPE FUNCTIONS
//
/////////////////////////////////////


// ENVELOPE 1

void MMusic::enableEnvelope1()
{
	envelopeOn1 = true;
}


void MMusic::disableEnvelope1()
{
	envelopeOn1 = false;
}


void MMusic::setEnv1Stage(uint8_t stage)
{
	env1Stage = stage;
}


void MMusic::setEnv1Attack(uint8_t att)
{
	if(att>127) att = 127;
	attack1 = envTimeTable[att];
}


void MMusic::setEnv1Decay(uint8_t dec)
{
	if(dec>127) dec = 127;
	decay1 = envTimeTable[dec];
}


void MMusic::setEnv1Sustain(uint8_t sus)
{
	sustain1 = ((sus * MAX_ENV_GAIN)/128);	
}


void MMusic::setEnv1Release(uint8_t rel)
{
	if(rel>127) rel = 127;
	release1 = envTimeTable[rel];
}


void MMusic::setEnv1VelSustain(uint8_t vel)
{
	velSustain1 = vel * (sustain1 / 128);	
}


void MMusic::setEnv1VelPeak(uint8_t vel)
{
	velPeak1 = vel * (MAX_ENV_GAIN / 128);	
}


// ENVELOPE 2

void MMusic::enableEnvelope2()
{
	envelopeOn2 = true;
}


void MMusic::disableEnvelope2()
{
	envelopeOn2 = false;
}


void MMusic::setEnv2Stage(uint8_t stage)
{
	env2Stage = stage;
}


void MMusic::setEnv2Attack(uint8_t att)
{
	if(att>127) att = 127;
	attack2 = envTimeTable[att];
}


void MMusic::setEnv2Decay(uint8_t dec)
{
	if(dec>127) dec = 127;
	decay2 = envTimeTable[dec];
}


void MMusic::setEnv2Sustain(uint8_t sus)
{
	sustain2 = ((sus * MAX_ENV_GAIN)/128);	
}


void MMusic::setEnv2Release(uint8_t rel)
{
	if(rel>127) rel = 127;
	release2 = envTimeTable[rel];
}


void MMusic::setEnv2VelSustain(uint8_t vel)
{
	velSustain2 = vel * (sustain2 / 128);	
}


void MMusic::setEnv2VelPeak(uint8_t vel)
{
	velPeak2 = vel * (MAX_ENV_GAIN / 128);	
}




#ifdef MIDI

/////////////////////////////////////
//
//	MIDI specific functions
//
/////////////////////////////////////

bool midiRead = false;

void MMidi::init()
{	
	Serial.begin(9600);
	
	midiBufferIndex = 0;
	midiChannel = MIDI_CHANNEL - 1;
	if(midiChannel < 0 || midiChannel > 15) midiChannel = 0;
	
}

void MMidi::checkMidi()
{
	//while(Serial.available() > 32) Serial.read();
	while(Serial.available() > 0) {
		
		data = Serial.read();
		
		if(data & 0x80 && (data & 0x0F) == midiChannel) {	// bitmask with 10000000 to see if byte is over 127 (data&0x80)
			midiBufferIndex = 0;							// and check if the midi channel corresponds to the midiChannel
			midiRead = true;								// the device is set to listen to.
		} else if(data & 0x80) {							// Else if the byte is over 127 (but not on the device's
			midiRead = false;								// midiChannel, don't read this or any following bytes.
		}
		
		if(midiRead) {
			midiBuffer[midiBufferIndex] = data;
			midiBufferIndex++;
			if (midiBufferIndex > 2) {
				midiRead = false;
				midiHandler();
			}
		}
	}	
}


void MMidi::midiHandler() {
	
    uint8_t midiChannel = (midiBuffer[0] & 0x0F);
    
	
	switch(midiBuffer[0] & 0xF0) { // bit mask with &0xF0 ?
        case 0x80:
			noteOff			(midiBuffer[0] & 0x0F,     // midi channel 0-16
							 midiBuffer[1] & 0x7F,   // note value 0-127
							 midiBuffer[2] & 0x7F);  // note velocity 0-127
			break;
			
        case 0x90:
			noteOn			(midiBuffer[0] & 0x0F,     // midi channel 0-16
							 midiBuffer[1] & 0x7F,   // note value 0-127
							 midiBuffer[2] & 0x7F);  // note velocity 0-127
			break;
			
        case 0xA0:
			aftertouch		(midiBuffer[0] & 0x0F,   // midi channel 0-16
							 midiBuffer[1] & 0x7F, // note value 0-127
							 midiBuffer[2] & 0x7F);// note velocity 0-127
			break;
			
        case 0xB0:
			controller		(midiBuffer[0] & 0x0F,   // midi channel 0-16
							 midiBuffer[1] & 0x7F, // controller number 0-127
							 midiBuffer[2] & 0x7F);// controller value 0-127
			break;
			
        case 0xC0:
			programChange	(midiBuffer[0]  & 0x0F,    // midi channel 0-16
							 midiBuffer[1] & 0x7F);  // program number 0-127
			break;
			
        case 0xD0:
			channelPressure	(midiBuffer[0]  & 0x0F,    // midi channel 0-16
							 midiBuffer[1] & 0x7F);  // pressure amount 0-127
			break;
			
        case 0xE0:
			pitchWheel		(midiBuffer[0] & 0x0F,   // midi channel 0-16
							 midiBuffer[1] & 0x7F, // higher bits 0-6
							 midiBuffer[2] & 0x7F);// lower bits 7-13
			break;
			
        default:
			break;
	}
}


void inline MMidi::noteOff(uint8_t channel, uint8_t note, uint8_t vel) {
	
	Music.noteOff(note);
}


void inline MMidi::noteOn(uint8_t channel, uint8_t note, uint8_t vel) {

	Music.noteOn(note, vel);
}


void inline MMidi::aftertouch(uint8_t channel, uint8_t note, uint8_t pressure) {
	// Write code here for Aftertouch 
}


void inline MMidi::controller(uint8_t channel, uint8_t number, uint8_t value) {

	switch(number) {
		case IS_12_BIT:
			Music.set12bit(value/64);
			break;
		case PORTAMENTO:
			Music.setPortamento(portamentoTimeTable[value]);
			break;
		case CUTOFF:
			Music.setCutoff(value * 256);
			break;			
		case ZERO_HZ_FM:
			if(value < 64) Music.fmToZeroHertz(false);
			else Music.fmToZeroHertz(true);
			break;
		case FM_OCTAVES:
			Music.setFMoctaves(value+1);
			break;
		case LFO1:
			Music.setOsc1LFO(value/64);
			break;
		case LFO2:
			Music.setOsc2LFO(value/64);
			break;
		case LFO3:
			Music.setOsc3LFO(value/64);
			break;
		case FREQUENCY1:
			if(Music.osc1LFO) Music.setFrequency1(Music.getNoteFrequency(value)/1024.0);
			else Music.setFrequency1(Music.getNoteFrequency(value));
			break;
		case FREQUENCY2:
			if(Music.osc2LFO) Music.setFrequency2(Music.getNoteFrequency(value)/1024.0);
			else Music.setFrequency2(Music.getNoteFrequency(value));
			break;
		case FREQUENCY3:
			if(Music.osc3LFO) Music.setFrequency3(Music.getNoteFrequency(value)/1024.0);
			else Music.setFrequency3(Music.getNoteFrequency(value));
			break;
		case DETUNE1:
			Music.setDetune1(map(value,0,127,-100,100)*0.0005946);
			break;
		case DETUNE2:
			Music.setDetune2(map(value,0,127,-100,100)*0.0005946);
			//Music.setDetune2((value-64.0)*0.0005946);
			//Music.setDetune2(value/5120.0);
			break;
		case DETUNE3:
			Music.setDetune3(map(value,0,127,-100,100)*0.0005946);							 
			//Music.setDetune3((value-64.0)*0.0005946);
			//Music.setDetune3(value/5120.0);
			break;
		case SEMITONE1:
			if(15 < value && value < 113) {
				int8_t val = (((value-16)/2)-24);
				Music.setSemitone1(val);
			} else if (value < 16) {
				Music.setSemitone1(-24);				
			} else {
				Music.setSemitone1(24);
			}
			break;
		case SEMITONE2:
			if(15 < value && value < 113) {
				int8_t val = (((value-16)/2)-24);
				Music.setSemitone2(val);
			} else if (value < 16) {
				Music.setSemitone2(-24);				
			} else {
				Music.setSemitone2(24);
			}
			break;
		case SEMITONE3:
			if(15 < value && value < 113) {
				int8_t val = (((value-16)/2)-24);
				Music.setSemitone3(val);
			} else if (value < 16) {
				Music.setSemitone3(-24);				
			} else {
				Music.setSemitone3(24);
			}
			break;
		case GAIN1:
			Music.setGain1(value / 127.0);
			break;
		case GAIN2:
			Music.setGain2(value / 127.0);
			break;
		case GAIN3:
			Music.setGain3(value / 127.0);
			break;
		case WAVEFORM1:
			Music.setWaveform1(value / 8);
			break;
		case WAVEFORM2:
			Music.setWaveform2(value / 8);
			break;
		case WAVEFORM3:
			Music.setWaveform3(value / 8);
			break;
		case FM1:
			Music.setFM1(value);
			break;
		case FM2:
			Music.setFM2(value);
			break;
		case FM3:
			Music.setFM3(value);
			break;
		case FM1_OCTAVES:
			Music.setFM1octaves(value+1);
			break;
		case FM2_OCTAVES:
			Music.setFM2octaves(value+1);
			break;
		case FM3_OCTAVES:
			Music.setFM3octaves(value+1);
			break;
		case FM1_SOURCE:
			Music.setFM1Source(value/32);
			break;
		case FM2_SOURCE:
			Music.setFM2Source(value/32);
			break;
		case FM3_SOURCE:
			Music.setFM3Source(value/32);
			break;
		case FM1_SHAPE:
			Music.setFM1Shape(value/32);
			break;
		case FM2_SHAPE:
			Music.setFM2Shape(value/32);
			break;
		case FM3_SHAPE:
			Music.setFM3Shape(value/32);
			break;
		case ENV1_ENABLE:
			if(value<64) Music.enableEnvelope1();
			else Music.disableEnvelope1();
			break;
		case ENV1_ATTACK:
			Music.setEnv1Attack(value);
			break;
		case ENV1_DECAY:
			Music.setEnv1Decay(value);
			break;
		case ENV1_SUSTAIN:
			Music.setEnv1Sustain(value);
			break;
		case ENV1_RELEASE:
			Music.setEnv1Release(value);
			break;
		case ENV2_ENABLE:
			if(value<64) Music.enableEnvelope2();
			else Music.disableEnvelope2();
			break;
		case ENV2_ATTACK:
			Music.setEnv2Attack(value);
			break;
		case ENV2_DECAY:
			Music.setEnv2Decay(value);
			break;
		case ENV2_SUSTAIN:
			Music.setEnv2Sustain(value);
			break;
		case ENV2_RELEASE:
			Music.setEnv2Release(value);
			break;
		default:
			break;
	} 
}


void inline MMidi::programChange(uint8_t channel, uint8_t number) {
	// Write code here for Program Change 
}


void inline MMidi::channelPressure(uint8_t channel, uint8_t pressure) {
	// Write code here for Channel Pressure 
}


void inline MMidi::pitchWheel(uint8_t channel, uint8_t highBits, uint8_t lowBits) {
	// Write code here for Pitch Wheel
}

#endif

#endif // close guard Friction_h

