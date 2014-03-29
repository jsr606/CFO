/* 
 Synth.cpp - Friction Music library
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

#include "Synth.h"

IntervalTimer synthTimer;

MCP4251 Mcp4251 = MCP4251( MCP4251_CS, 100000.0 ); 

MMusic Music;

MMidi Midi;

const uint16_t sineTable[] = { 
#include <FrictionSineTable16bitHex.inc>
};

const uint16_t waveTable[] = { 
#include <FrictionWaveTable.inc>
};

// Table of MIDI note values to frequency in Hertz
const float hertzTable[] = {
#include <FrictionHertzTable.inc>	
};

// Used in the functions that set the envelope timing
const uint32_t envTimeTable[] = {1,5,9,14,19,26,34,42,53,65,79,95,113,134,157,182,211,243,278,317,359,405,456,511,570,633,702,776,854,939,1029,1124,1226,1333,1448,1568,1695,1829,1971,2119,2274,2438,2610,2789,2977,3172,3377,3590,3813,4044,4285,4535,4795,5065,5345,5635,5936,6247,6569,6902,7247,7602,7970,8349,8740,9143,9559,9986,10427,10880,11347,11827,12321,12828,13349,13883,14433,14996,15574,16167,16775,17398,18036,18690,19359,20045,20746,21464,22198,22949,23716,24501,25303,26122,26959,27813,28686,29577,30486,31413,32359,33325,34309,35312,36335,37378,38440,39522,40625,41748,42892,44056,45241,46448,47675,48925,50196,51489,52803,54141,55500,56883,58288,59716,61167,62642,64140,65662};

const float semitoneTable[] = {0.25,0.2648658,0.2806155,0.29730177,0.31498027,0.33370996,0.35355338,0.37457678,0.39685026,0.4204482,0.44544938,0.47193715,0.5,0.5297315,0.561231,0.59460354,0.62996054,0.6674199,0.70710677,0.74915355,0.7937005,0.8408964,0.8908987,0.9438743,1.0,1.0594631,1.122462,1.1892071,1.2599211,1.3348398,1.4142135,1.4983071,1.587401,1.6817929,1.7817974,1.8877486,2.0,2.1189263,2.244924,2.3784142,2.5198421,2.6696796,2.828427,2.9966142,3.174802,3.3635857,3.563595,3.7754972,4.0};

const extern uint32_t portamentoTimeTable[] = {1,5,9,13,17,21,26,30,35,39,44,49,54,59,64,69,74,79,85,90,96,101,107,113,119,125,132,138,144,151,158,165,172,179,187,194,202,210,218,226,234,243,252,261,270,279,289,299,309,320,330,341,353,364,376,388,401,414,427,440,455,469,484,500,516,532,549,566,584,602,622,642,663,684,706,729,753,778,804,831,859,888,919,951,984,1019,1056,1094,1134,1176,1221,1268,1317,1370,1425,1484,1547,1614,1684,1760,1841,1929,2023,2125,2234,2354,2484,2627,2785,2959,3152,3368,3611,3886,4201,4563,4987,5487,6087,6821,7739,8918,10491,12693,15996,21500,32509,65535};


//////////////////////////////////////////////////////////
//
// SYNTH INTERRUPT - The pre-processor selects 8 or 12 bit
//
//////////////////////////////////////////////////////////

void synth_isr(void) {
	
    Music.output2T3DAC();
//	Music.sendSampleToDAC();
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
//	sample += 32768;

}





/////////////////////////////////////////////////////////
//
//	12 BIT OSCILLATOR - SINETABLE
//
/////////////////////////////////////////////////////////


void MMusic::synthInterrupt12bitSineFM()
{	
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
//	sample += 32768;
 	
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
/*	
	dacSPIB0 = env1 >> 8;
	dacSPIB0 >>= 4;
	dacSPIB0 |= dacSetB; 
	dacSPIB1 = env1 >> 4;
	
	digitalWriteFast(DAC_CS, LOW);
    spi4teensy3::send(dacSPIB0);
    spi4teensy3::send(dacSPIB1);
    
	//	while(SPI.transfer(dacSPIB0));
	//	while(SPI.transfer(dacSPIB1));
	digitalWriteFast(DAC_CS, HIGH);
*/	

}


/////////////////////////////////////////////////////////
//
//	SEND SAMPLE TO DAC
//
/////////////////////////////////////////////////////////


void MMusic::sendSampleToDAC() {

	sample += 32768;

	// Formatting the samples to be transfered to the MCP4921 DAC to output A
	dacSPIA0 = sample >> 8;
	dacSPIA0 >>= 4;
	dacSPIA0 |= dacSetA; 
	dacSPIA1 = sample >> 4;

	digitalWriteFast(DAC_CS, LOW);
    spi4teensy3::send(dacSPIA0);
    spi4teensy3::send(dacSPIA1);
    
//	while(SPI.transfer(dacSPIA0));
//	while(SPI.transfer(dacSPIA1));
	digitalWriteFast(DAC_CS, HIGH);

}

void MMusic::output2T3DAC() {
	sample += 32768;
    analogWrite(A14, sample>>4);
}




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
    spi4teensy3::init(0,0,0);
	pinMode(DAC_CS, OUTPUT);
    //	SPI.begin();
//	SPI.setClockDivider(SPI_CLOCK_DIV16);  // T3_Beta7 requires this, no default?	
}


void MMusic::init()
{
    pinMode(MUX_A, OUTPUT);
    pinMode(MUX_B, OUTPUT);
    
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
	
	osc1modSource_ptr = &oscil3;
	osc2modSource_ptr = &oscil1;
	osc3modSource_ptr = &oscil2;
	osc1modShape_ptr = &fullSignal;
	osc2modShape_ptr = &fullSignal;
	osc3modShape_ptr = &fullSignal;

	amp_modSource_ptr = &env1;
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
		
	// DAC setup
	dacSetA = 0;
	dacSetB = 0;
	dacSetA |= (DAC_A << DAC_AB) | (0 << DAC_BUF) | (1 << DAC_GA) | (1 << DAC_SHDN);
	dacSetB |= (DAC_B << DAC_AB) | (0 << DAC_BUF) | (1 << DAC_GA) | (1 << DAC_SHDN);
    
    analogWriteResolution(12);
	
	spi_setup();

	// filter setup
	setCutoff(4095);
	setResonance(127);
    setFilterType(0);
	
	cutoffModSource_ptr = &env2;
	resonanceModSource_ptr = &fullSignal;
	cutoffModShape_ptr = &fullSignal;
	resonanceModShape_ptr = &fullSignal;

	setCutoffModSource(2);
//	cutoffModSource_ptr = &fullSignal;
//	setCutoffModShape(0);
	setCutoffModAmount(BIT_16);
	setCutoffModDirection(1);
	
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


void MMusic::setResonance(uint8_t res)
{
	resonance = res;
}


void MMusic::setCutoffModAmount(int32_t amount) {
	if(amount >= 65536) cutoffModAmount = 65535;
	else if(amount < -65536) cutoffModAmount = -65536;
	else cutoffModAmount = amount;
//	cutoffModAmount = amount;
}

void MMusic::setCutoffModDirection(int32_t direction) {
	if(direction >= 0) cutoffModDirection = 1;
	else cutoffModDirection = -1;
}

void MMusic::monotronFilter() {
	uint32_t cutoffValue = env2 >> 6;
	analogWrite(CUTOFF_PIN, cutoffValue);
}


void MMusic::filter() {
	
	
//	uint32_t c = (env2 * ((cutoff + 65536) >> 1)) >> 16;
	//	int64_t mod = (cutoffModAmount * (*cutoffModSource_ptr))>>16;
	int64_t mod = (int64_t(cutoffModAmount) * (int64_t(*cutoffModSource_ptr)))>>16;
	int64_t c = (mod + int64_t(cutoff))>>1;
	if(c > 65535) c = 65535;
	else if(c < 0) c = 0;
	c = ((((c * 32768) >> 15) + 65536) >> 1);
//	c = ((((c * 65535) >> 15) + 65536) >> 1);
	
//	uint32_t c = cutoff;
	
	// Formatting the samples to be transfered to the MCP4822 DAC to output B
	dacSPIA0 = uint32_t(c) >> 8;
	dacSPIA0 >>= 4;
	dacSPIA0 |= dacSetA; 
	dacSPIA1 = c >> 4;
	
	digitalWriteFast(DAC_CS, LOW);
    spi4teensy3::send(dacSPIA0);
    spi4teensy3::send(dacSPIA1);
//	while(SPI.transfer(dacSPIA0));
//	while(SPI.transfer(dacSPIA1));
	digitalWriteFast(DAC_CS, HIGH);

//	Mcp4251.wiper0_pos(resonance);
	Mcp4251.wiper1_pos(resonance);

}


void MMusic::setFilterType(uint8_t type) {
    
    if(type == LOWPASS) {
        digitalWrite(MUX_B, LOW);
        digitalWrite(MUX_A, LOW);
    }
    else if(type == HIGHPASS) {
        digitalWrite(MUX_B, LOW);
        digitalWrite(MUX_A, HIGH);
    }
    else if(type == BANDPASS) {
        digitalWrite(MUX_B, HIGH);
        digitalWrite(MUX_A, LOW);
    }
    else if(type == NOTCH) {
        digitalWrite(MUX_B, HIGH);
        digitalWrite(MUX_A, HIGH);
    }
}


void MMusic::setCutoffModShape(uint8_t shape) {
	switch(shape) {
		case 0:
			cutoffModShape_ptr = &fullSignal;
			break;
		case 1:
			cutoffModShape_ptr = &env1;
			break;
		case 2:
			cutoffModShape_ptr = &env2;
			break;
		case 3:
			cutoffModShape_ptr = &oscil1;
			break;
		case 4:
			cutoffModShape_ptr = &oscil2;
			break;
		case 5:
			cutoffModShape_ptr = &oscil3;
			break;
		default:
			cutoffModShape_ptr = &fullSignal;
			break;
	}
}


void MMusic::setCutoffModSource(uint8_t source) {
	switch(source) {
		case 0:
			cutoffModSource_ptr = &fullSignal;
			break;
		case 1:
			cutoffModSource_ptr = &env1;
			break;
		case 2:
			cutoffModSource_ptr = &env2;
			break;
		case 3:
			cutoffModSource_ptr = &oscil1;
			break;
		case 4:
			cutoffModSource_ptr = &oscil2;
			break;
		case 5:
			cutoffModSource_ptr = &oscil3;
			break;
		default:
			cutoffModSource_ptr = &fullSignal;
			break;
	}
}


void MMusic::setResonanceModShape(uint8_t shape) {
	switch(shape) {
		case 0:
			resonanceModShape_ptr = &fullSignal;
			break;
		case 1:
			resonanceModShape_ptr = &env1;
			break;
		case 2:
			resonanceModShape_ptr = &env2;
			break;
		case 3:
			resonanceModShape_ptr = &oscil1;
			break;
		case 4:
			resonanceModShape_ptr = &oscil2;
			break;
		case 5:
			resonanceModShape_ptr = &oscil3;
			break;
		default:
			resonanceModShape_ptr = &fullSignal;
			break;
	}
}


void MMusic::setResonanceModSource(uint8_t source) {
	switch(source) {
		case 0:
			resonanceModSource_ptr = &fullSignal;
			break;
		case 1:
			resonanceModSource_ptr = &env1;
			break;
		case 2:
			resonanceModSource_ptr = &env2;
			break;
		case 3:
			resonanceModSource_ptr = &oscil1;
			break;
		case 4:
			resonanceModSource_ptr = &oscil2;
			break;
		case 5:
			resonanceModSource_ptr = &oscil3;
			break;
		default:
			resonanceModSource_ptr = &fullSignal;
			break;
	}
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
		case 3:
			osc1modShape_ptr = &oscil1;
			break;
		case 4:
			osc1modShape_ptr = &oscil2;
			break;
		case 5:
			osc1modShape_ptr = &oscil3;
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
		case 3:
			osc2modShape_ptr = &oscil1;
			break;
		case 4:
			osc2modShape_ptr = &oscil2;
			break;
		case 5:
			osc2modShape_ptr = &oscil3;
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
		case 3:
			osc3modShape_ptr = &oscil1;
			break;
		case 4:
			osc3modShape_ptr = &oscil2;
			break;
		case 5:
			osc3modShape_ptr = &oscil3;
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


void MMidi::noteOff(uint8_t channel, uint8_t note, uint8_t vel) {
	
	Music.noteOff(note);
}


void MMidi::noteOn(uint8_t channel, uint8_t note, uint8_t vel) {
	
	Music.noteOn(note, vel);
}


void MMidi::aftertouch(uint8_t channel, uint8_t note, uint8_t pressure) {
	// Write code here for Aftertouch 
}


void MMidi::controller(uint8_t channel, uint8_t number, uint8_t value) {
	
	switch(number) {
		case IS_12_BIT:
			Music.set12bit(value/64);
			break;
		case PORTAMENTO:
			Music.setPortamento(portamentoTimeTable[value]);
			break;
		case CUTOFF:
			Music.setCutoff(value * 512);
			break;			
		case RESONANCE:
			Music.setResonance(value * 2);
			break;			
		case FILTER_TYPE:
			Music.setFilterType(value/32);
			break;
		case CUTOFF_MOD_AMOUNT:
			Music.setCutoffModAmount((value-64) * 1024);
			//			if(value > 63) Music.setCutoffModAmount((value-64) * 512);
			//			if(value < 64) Music.setCutoffModAmount(() * 512);
			break;
			//		case CUTOFF_MOD_DIRECTION:
			//			Music.setCutoffModDirection(value);
			//			break;			
		case CUTOFF_SOURCE:
			Music.setCutoffModSource(value);
			break;			
		case CUTOFF_SHAPE:
			Music.setCutoffModShape(value);
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
			Music.setFM1Source(value);
			break;
		case FM2_SOURCE:
			Music.setFM2Source(value);
			break;
		case FM3_SOURCE:
			Music.setFM3Source(value);
			break;
		case FM1_SHAPE:
			Music.setFM1Shape(value);
			break;
		case FM2_SHAPE:
			Music.setFM2Shape(value);
			break;
		case FM3_SHAPE:
			Music.setFM3Shape(value);
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


void MMidi::programChange(uint8_t channel, uint8_t number) {
	// Write code here for Program Change 
}


void MMidi::channelPressure(uint8_t channel, uint8_t pressure) {
	// Write code here for Channel Pressure 
}


void MMidi::pitchWheel(uint8_t channel, uint8_t highBits, uint8_t lowBits) {
	// Write code here for Pitch Wheel
}

