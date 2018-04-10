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

#include "BodyseqSynth.h"

IntervalTimer synthTimer;

MMusic Music;

MMidi Midi;


int envelopeMultiplier = 5, currentEnvelopeCycle = 0;

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

uint8_t instrument[128];
uint8_t userPresets[MAX_PRESETS][PRESET_SIZE];

const uint8_t programPresets[] = {
#include <Presets.h>
};

int64_t filterSamplesLP24dB[4];
int64_t filterSamplesHP24dB[8];
int64_t filterSamplesMoogLadder[4];

bool samplePlaying[NUM_SAMPLES];
int samplePosition[NUM_SAMPLES];

const uint8_t sample0[] = {
//#include <sample_lachute.inc>
#include <sample_BD2.inc>
};
const uint8_t sample1[] = {
//#include <sample_silence.inc>
#include <sample_SD.inc>
};
const uint8_t sample2[] = {
//#include <sample_silence.inc>
#include <sample_RS.inc>
};
const uint8_t sample3[] = {
//#include <sample_silence.inc>
#include <sample_CP.inc>
};
const uint8_t sample4[] = {
//#include <sample_silence.inc>
#include <sample_HH.inc>
};
const uint8_t sample5[] = {
//#include <sample_silence.inc>
#include <sample_OH.inc>
};
const uint8_t sample6[] = {
//#include <sample_silence.inc>
#include <sample_CL.inc>
};
const uint8_t sample7[] = {
//#include <sample_silence.inc>
#include <sample_CB.inc>
};
// const uint8_t sample8[] = {
// #include <sample_silence.inc>
// };
// const uint8_t sample9[] = {
// #include <sample_silence.inc>
// };
// const uint8_t sample10[] = {
// #include <sample_silence.inc>
// };
// const uint8_t sample11[] = {
// #include <sample_silence.inc>
// };
// const uint8_t sample12[] = {
// #include <sample_silence.inc>
// };
// const uint8_t sample13[] = {
// #include <sample_silence.inc>
// };
// const uint8_t sample14[] = {
// #include <sample_silence.inc>
// };
// const uint8_t sample15[] = {
// #include <sample_silence.inc>
// };
int sampleLength[] =   {
                            sizeof(sample0) / sizeof(sample0[0]),
                            sizeof(sample1) / sizeof(sample1[0]),
                            sizeof(sample2) / sizeof(sample2[0]),
                            sizeof(sample3) / sizeof(sample3[0]),
                            sizeof(sample4) / sizeof(sample4[0]),
                            sizeof(sample5) / sizeof(sample5[0]),
                            sizeof(sample6) / sizeof(sample6[0]),
                            sizeof(sample7) / sizeof(sample7[0])
                            // sizeof(sample8) / sizeof(sample8[0]),
                            // sizeof(sample9) / sizeof(sample9[0]),
                            // sizeof(sample10) / sizeof(sample10[0]),
                            // sizeof(sample11) / sizeof(sample11[0]),
                            // sizeof(sample12) / sizeof(sample12[0]),
                            // sizeof(sample13) / sizeof(sample13[0]),
                            // sizeof(sample14) / sizeof(sample14[0]),
                            // sizeof(sample15) / sizeof(sample15[0])
                        };

const int64_t filterCoefficient[] = {
#include <filterCoefficients_1poleLP.inc>
};

const float fcMoog[] = {
#include <filterCutoffFrequenciesMoogLadder.inc>
};

const int64_t filterCoefficientsMoogLadder[] = {
#include <filterCoefficientsMoogLadder.inc>
};


// Used in the functions that set the envelope timing
const uint32_t envTimeTable[] = {1,5,9,14,19,26,34,42,53,65,79,95,113,134,157,182,211,243,278,317,359,405,456,511,570,633,702,776,854,939,1029,1124,1226,1333,1448,1568,1695,1829,1971,2119,2274,2438,2610,2789,2977,3172,3377,3590,3813,4044,4285,4535,4795,5065,5345,5635,5936,6247,6569,6902,7247,7602,7970,8349,8740,9143,9559,9986,10427,10880,11347,11827,12321,12828,13349,13883,14433,14996,15574,16167,16775,17398,18036,18690,19359,20045,20746,21464,22198,22949,23716,24501,25303,26122,26959,27813,28686,29577,30486,31413,32359,33325,34309,35312,36335,37378,38440,39522,40625,41748,42892,44056,45241,46448,47675,48925,50196,51489,52803,54141,55500,56883,58288,59716,61167,62642,64140,65662};

const float semitoneTable[] = {0.25,0.2648658,0.2806155,0.29730177,0.31498027,0.33370996,0.35355338,0.37457678,0.39685026,0.4204482,0.44544938,0.47193715,0.5,0.5297315,0.561231,0.59460354,0.62996054,0.6674199,0.70710677,0.74915355,0.7937005,0.8408964,0.8908987,0.9438743,1.0,1.0594631,1.122462,1.1892071,1.2599211,1.3348398,1.4142135,1.4983071,1.587401,1.6817929,1.7817974,1.8877486,2.0,2.1189263,2.244924,2.3784142,2.5198421,2.6696796,2.828427,2.9966142,3.174802,3.3635857,3.563595,3.7754972,4.0};

const extern uint32_t portamentoTimeTable[] = {1,5,9,13,17,21,26,30,35,39,44,49,54,59,64,69,74,79,85,90,96,101,107,113,119,125,132,138,144,151,158,165,172,179,187,194,202,210,218,226,234,243,252,261,270,279,289,299,309,320,330,341,353,364,376,388,401,414,427,440,455,469,484,500,516,532,549,566,584,602,622,642,663,684,706,729,753,778,804,831,859,888,919,951,984,1019,1056,1094,1134,1176,1221,1268,1317,1370,1425,1484,1547,1614,1684,1760,1841,1929,2023,2125,2234,2354,2484,2627,2785,2959,3152,3368,3611,3886,4201,4563,4987,5487,6087,6821,7739,8918,10491,12693,15996,21500,32509,65535};



//////////////////////////////////////////////////////////
//
// SYNTH INTERRUPT
//
//////////////////////////////////////////////////////////

void synth_isr(void) {

    Music.output2T3DAC();
    //    Music.output2DAC();

    if(Music.isSynth()) {

    	// envelope delay
    	if (currentEnvelopeCycle % envelopeMultiplier == 0) {
        	Music.envelope1();
        	Music.envelope2();
    	}
    	currentEnvelopeCycle++;

        if(Music.is12bit) Music.synthInterrupt12bitSineFM();
    //  if(Music.is12bit) Music.synthInterrupt12bitSawFM();
        else Music.synthInterrupt8bitFM();
        
        Music.amplifier();
        
        if(Music.lowpass) Music.filterLP6dB();
        if(Music.highpass) Music.filterHP6dB();
        if(Music.lowpass24dB) Music.filterLP24dB();
        if(Music.highpass24dB) Music.filterHP24dB();
        if(Music.moogLadder) Music.filterMoogLadder();
        
    }
    if(Music.isSampler()) {
        if(!Music.isSynth()) {
            Music.sample = 0;
        }
        Music.samplerInterrupt();
    }
    
    
    if(Music.isSynth() && !Music.isSampler());
    else if(!Music.isSynth() && Music.isSampler()) Music.sample >>= 0;
    else Music.sample >>= 1;


}


void MMusic::set12bit(bool b) {
	is12bit = b;
}
	

void MMusic::setBitcrush(int b) {
	if(b < 0) b = 0;
	else if(b > 7) b = 7;
	bitcrush = b + 8;
}

void MMusic::samplerInterrupt()
{
    sample += ((int(sample0[samplePosition[0]]) - 128) << 8);
    sample += ((int(sample1[samplePosition[1]]) - 128) << 8);
    sample += ((int(sample2[samplePosition[2]]) - 128) << 8);
    sample += ((int(sample3[samplePosition[3]]) - 128) << 8);
    sample += ((int(sample4[samplePosition[4]]) - 128) << 8);
    sample += ((int(sample5[samplePosition[5]]) - 128) << 8);
    sample += ((int(sample6[samplePosition[6]]) - 128) << 8);
    sample += ((int(sample7[samplePosition[7]]) - 128) << 8);
    // sample += ((int(sample8[samplePosition[8]]) - 128) << 8);
    // sample += ((int(sample9[samplePosition[9]]) - 128) << 8);
    // sample += ((int(sample10[samplePosition[10]]) - 128) << 8);
    // sample += ((int(sample11[samplePosition[11]]) - 128) << 8);
    // sample += ((int(sample12[samplePosition[12]]) - 128) << 8);
    // sample += ((int(sample13[samplePosition[13]]) - 128) << 8);
    // sample += ((int(sample14[samplePosition[14]]) - 128) << 8);
    // sample += ((int(sample15[samplePosition[15]]) - 128) << 8);
    sample = ((sample >> bitcrush) << bitcrush);
    // if(bitcrush > 5) sample >>= 1;
    // if(bitcrush > 6) sample >>= 1;

    for(int i=0; i<NUM_SAMPLES; i++) {
        if(samplePlaying[i]) {
            samplePosition[i]++;
            if(samplePosition[i] >= sampleLength[i]) {
                samplePlaying[i] = false;
                samplePosition[i] = 0;
            }
        }
    }
}



/////////////////////////////////////////////////////////
//
//	8 BIT OSCILLATOR - WAVETABLE - 16 WAVEFORMS
//
/////////////////////////////////////////////////////////



void MMusic::synthInterrupt8bitFM ()
{
	
	dPhase1 = dPhase1 + (period1 - dPhase1) / portamento;
	modulator1 = (fmAmount1 * fmOctaves1 * (*osc1modSource_ptr))>>10;
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

}




/////////////////////////////////////////////////////////
//
//	12 BIT OSCILLATOR - SINETABLE
//
/////////////////////////////////////////////////////////


void MMusic::synthInterrupt12bitSineFM()
{
	
	dPhase1 = dPhase1 + (period1 - dPhase1) / portamento;
	modulator1 = (fmAmount1 * fmOctaves1 * (*osc1modSource_ptr))>>10;
	modulator1 = (modulator1 * (*osc1modShape_ptr))>>16;
	modulator1 = (modulator1 * int64_t(dPhase1))>>16;
	modulator1 = (modulator1>>((modulator1>>31)&zeroFM));
	accumulator1 = accumulator1 + dPhase1 + modulator1;
	index1 = accumulator1 >> 20;
	oscil1 = sineTable[index1];
	index1 = accumulator1 >> 20;
	oscil1 -= 32768;
	sample = (oscil1 * gain1);
	
	dPhase2 = dPhase2 + (period2 - dPhase2) / portamento;
	modulator2 = (fmAmount2 * fmOctaves2 * (*osc2modSource_ptr))>>10;
	modulator2 = (modulator2 * (*osc2modShape_ptr))>>16;
	modulator2 = (modulator2 * int64_t(dPhase2))>>16;
	modulator2 = (modulator2>>((modulator2>>31)&zeroFM));
	accumulator2 = accumulator2 + dPhase2+ modulator2;
	index2 = accumulator2 >> 20;
	oscil2 = sineTable[index2];
	oscil2 -= 32768;
	sample += (oscil2 * gain2); 
	
	dPhase3 = dPhase3 + (period3 - dPhase3) / portamento;
	modulator3 = (fmAmount3 * fmOctaves3 * (*osc3modSource_ptr))>>10;
	modulator3 = (modulator3 * (*osc3modShape_ptr))>>16;
	modulator3 = (modulator3 * int64_t(dPhase3))>>16;
	modulator3 = (modulator3>>((modulator3>>31)&zeroFM));
	accumulator3 = accumulator3 + dPhase3 + modulator3;
	index3 = accumulator3 >> 20;
	oscil3 = sineTable[index3];
	oscil3 -= 32768;
	sample += (oscil3 * gain3);
	
	sample >>= 18;
 	
}


void MMusic::synthInterrupt12bitSawFM()
{
	
	dPhase1 = dPhase1 + (period1 - dPhase1) / portamento;
	modulator1 = (fmAmount1 * fmOctaves1 * (*osc1modSource_ptr))>>10;
	modulator1 = (modulator1 * (*osc1modShape_ptr))>>16;
	modulator1 = (modulator1 * int64_t(dPhase1))>>16;
	modulator1 = (modulator1>>((modulator1>>31)&zeroFM));
	accumulator1 = accumulator1 + dPhase1 + modulator1;
    //	index1 = accumulator1 >> 20;
    //	oscil1 = sineTable[index1];
	oscil1 = accumulator1 >> 16;
	oscil1 -= 32768;
	sample = (oscil1 * gain1);
	
	dPhase2 = dPhase2 + (period2 - dPhase2) / portamento;
	modulator2 = (fmAmount2 * fmOctaves2 * (*osc2modSource_ptr))>>10;
	modulator2 = (modulator2 * (*osc2modShape_ptr))>>16;
	modulator2 = (modulator2 * int64_t(dPhase2))>>16;
	modulator2 = (modulator2>>((modulator2>>31)&zeroFM));
	accumulator2 = accumulator2 + dPhase2+ modulator2;
    //	index2 = accumulator2 >> 20;
    //	oscil2 = sineTable[index2];
	oscil2 = accumulator2 >> 16;
	oscil2 -= 32768;
	sample += (oscil2 * gain2);
	
	dPhase3 = dPhase3 + (period3 - dPhase3) / portamento;
	modulator3 = (fmAmount3 * fmOctaves3 * (*osc3modSource_ptr))>>10;
	modulator3 = (modulator3 * (*osc3modShape_ptr))>>16;
	modulator3 = (modulator3 * int64_t(dPhase3))>>16;
	modulator3 = (modulator3>>((modulator3>>31)&zeroFM));
	accumulator3 = accumulator3 + dPhase3 + modulator3;
    //	index3 = accumulator3 >> 20;
    //	oscil3 = sineTable[index3];
	oscil3 = accumulator3 >> 16;
	oscil3 -= 32768;
	sample += (oscil3 * gain3);
	
	sample >>= 18;
 	
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


void MMusic::setSampler(bool s) {
    sampler = s;
}


bool MMusic::isSampler() {
    return sampler;
}


void MMusic::setSynth(bool s) {
    synth = s;
}


bool MMusic::isSynth() {
    return synth;
}



/////////////////////////////////////////////////////////
//
//	SEND SAMPLE TO DAC ON TEENSY 3.1 PIN A14
//
/////////////////////////////////////////////////////////


void MMusic::output2T3DAC() {
	sample += 32768;
    analogWrite(A14, sample>>4);
}

void MMusic::output2DAC() {
    sample += 32768;
    dacSPIA0 = sample >> 8;
	dacSPIA0 >>= 4;
	dacSPIA0 |= dacSetA;
	dacSPIA1 = sample >> 4;
	
	digitalWriteFast(DAC_CS, LOW);
    spi4teensy3::send(dacSPIA0);
    spi4teensy3::send(dacSPIA1);
    
	//	while(SPI.transfer(dacSPIB0));
	//	while(SPI.transfer(dacSPIB1));
	digitalWriteFast(DAC_CS, HIGH);
}





/////////////////////////////////////
//
//	INITIALIZING FUNCTION
//
/////////////////////////////////////


void MMusic::spi_setup()
{
    spi4teensy3::init(0,0,0);
	pinMode(DAC_CS, OUTPUT);
}

void MMusic::setEnvelopeMultiplier(uint8_t p)
{
	envelopeMultiplier = p;
}


void MMusic::getPreset(uint8_t p)
{
//	cli();
	if(p < MAX_PRESETS) {
//		Serial.print("GETTING PRESET NUMBER : ");
//		Serial.println(p);
		for(uint8_t i=2; i<128; i++) {
			instrument[i] = userPresets[p][i];
			Midi.controller(Midi.midiChannel, i, instrument[i]);
//			Serial.println(userPresets[p][i]);
		}
	} else {
		for(uint8_t i=2; i<128; i++) {
			instrument[i] = programPresets[(p-MAX_PRESETS)*PRESET_SIZE + i];
			Midi.controller(Midi.midiChannel, i, instrument[i]);
			//			Serial.println(userPresets[p][i]);
		}
		
	}

//	sei();
}

void MMusic::getRandomizedPreset(uint8_t p, uint8_t r)
{

	Serial.print("randomize preset ");
	Serial.print(p);
	Serial.print(" by ");
	Serial.print(r);
	Serial.println("%");

	// cli();
	
	// load preset values

	if(p < MAX_PRESETS) {
		for(uint8_t i=2; i<128; i++) {
			instrument[i] = userPresets[p][i];
		}
	} else {
		for(uint8_t i=2; i<128; i++) {
			instrument[i]=programPresets[(p-MAX_PRESETS)*PRESET_SIZE+i];
		}
		
	}

	// apply randomization
	
	for(uint8_t i=2; i<128; i++) {

		uint8_t presetValue = instrument[i];

		int ran = 0;
		
		// randomization only affects the following settings.
		// randomizing all paramaters will make a lot of unusable sounds.

		// 4: cutoff, 70: cutoff mod, 6: fm oct, 8: portamento
		if (i==4||i==70||i==6||i==8) {
			ran = random(float(r)/100*-127,float(r)/100*127);
		}
		
		// 10: LFO1, 12: detune1, 13: gain1, 15: FM1
		// 20: LFO2, 22: detune2, 23: gain2, 25: FM2
		// 30: LFO3, 32: detune3, 33: gain3, 35: FM3
		if (i==10||i==12||i==13||i==15||i==20||i==22||i==23||i==25||i==30||i==32||i==33||i==35) {
			ran = random(float(r)/100*-127,float(r)/100*127);
		}

		// 114: attack1, 115: decay1, 116: sustain1, 117: release1
		// 124: attack2, 125: decay2, 126: sustain2, 127: release2
		if (i==114||i==115||i==116||i==117||i==124||i==125||i==126||i==127) {
			ran = random(float(r)/100*-127,float(r)/100*127);
		}

		uint8_t newVal = constrain(presetValue+ran,0,127);			
		
		if (ran != 0)
		{
			/*
			Serial.print(i);
			Serial.print(": preset value ");
			Serial.print(presetValue);	
			Serial.print(" randomized by ");
			Serial.print(ran);
			Serial.print(" yielding ");
			Serial.println(newVal);
			*/
		}

		instrument[i] = newVal;		

		Midi.controller(Midi.midiChannel, i, instrument[i]);
	}

	// sei();

}


#if defined(USB_MIDI)

void MMusic::sendInstrument()
{
//	Serial.print("SENDING PRESET NUMBER : ");
//	Serial.print(p);
//	Serial.println(" OVER MIDI");
	cli();
	for(uint8_t i=2; i<128; i++) {
		usbMIDI.sendControlChange(i, instrument[i], Midi.midiChannel+1);
	}
	sei();
}

#else 

void MMusic::sendInstrument(){;}

#endif


void MMusic::savePreset(uint8_t p)
{
	if(p < MAX_PRESETS) {
		Serial.print("SAVING PRESET NUMBER : ");
		Serial.println(p);
		for(uint8_t i=0; i<128; i++) {
			//Serial.print(i);
			//Serial.print(" : ");
			userPresets[p][i] = instrument[i];
			//Serial.println(userPresets[p][i]);
			//insert code for saving instrument sequence here
			cli();
			EEPROM.write(p * PRESET_SIZE + i, instrument[i]);
			sei();
		}
	}
	else {
		Serial.println("CAN NOT SAVE PRESET TO EEPROM - COPY/PASTE BELOW TO FILE");
		for(uint8_t i=0; i<128; i++) {
			Serial.print(instrument[i]);
			Serial.print(", ");
		}
	}

}


void MMusic::loadAllPresets()
{
	for(uint8_t i=2; i<128; i++) {
		for(uint8_t p=0; p<MAX_PRESETS;p++) {
			userPresets[p][i] = EEPROM.read(p * PRESET_SIZE + i);
		}		
	}
}

void MMusic::init()
{
//    pinMode(MUX_A, OUTPUT);
//    pinMode(MUX_B, OUTPUT);

    Midi.init();
    
    setSynth(true);
    setSampler(false);
//    delay(2000);
//    
//    Serial.print("Sample1 is ");
//    Serial.print(sizeof(sample0));
//    Serial.println(" kbytes.");
//    Serial.print("Size of array is ");
//    Serial.println(sizeof(sample0) / sizeof(sample0[0]));
//    Serial.println(sizeof(sample1) / sizeof(sample1[0]));
//    Serial.println(sizeof(sample2) / sizeof(sample2[0]));
//    Serial.println(sizeof(sample3) / sizeof(sample3[0]));
//    Serial.println(sizeof(sample4) / sizeof(sample4[0]));
//    Serial.println(sizeof(sample5) / sizeof(sample5[0]));
//    Serial.println(sizeof(sample6) / sizeof(sample6[0]));
//    Serial.println(sizeof(sample7) / sizeof(sample7[0]));

    
    for(int i=0; i < NUM_SAMPLES; i++) {
        samplePlaying[i] = 0;
        samplePosition[i] = 0;
    }
	
	for(uint8_t i=0; i<128; i++) {
		instrument[i] = 0;
		for(uint8_t p=0; p<MAX_PRESETS;p++) {
			userPresets[p][i] = 0;
		}
	}
    
//    generateFilterCoefficientsMoogLadder();
//    for(int i=0; i<256; i++) {
//        Serial.println(filterCoefficientsMoogLadderFloat[6][i]);
//    }
    
	sampleRate = SAMPLE_RATE;
	sample = 0;
	set12bit(false);
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
	
	fmToZeroHertz(true);
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
//    analogWrite(A14, 0);  //Set the DAC output to 0.
////    DAC0_C0 &= 0b10111111;  //uses 1.2V reference for DAC instead of 3.3V
//    
//    SIM_SCGC2 |= SIM_SCGC2_DAC0;
//	DAC0_C0 = DAC_C0_DACEN;                   // 1.2V VDDA is DACREF_2
////	DAC0_C0 = DAC_C0_DACEN | DAC_C0_DACRFS; // 3.3V VDDA is DACREF_2
//	// slowly ramp up to DC voltage, approx 1/4 second
//	for (int16_t i=0; i<128; i++) {
//		analogWrite(A14, i);
//		delay(2);
//	}
	
	loadAllPresets();
	
//	spi_setup();

	// filter setup
	setCutoff(BIT_16);
	setResonance(BIT_16);
    setFilterType(0);
	
	cutoffModSource_ptr = &env2;
	resonanceModSource_ptr = &fullSignal;
	cutoffModShape_ptr = &fullSignal;
	resonanceModShape_ptr = &fullSignal;

	setCutoffModSource(2);
	setCutoffModAmount(BIT_16);
	setCutoffModDirection(1);
	
	cli();
	synthTimer.begin(synth_isr, 1000000.0 / sampleRate);
	sei();
    
}




/////////////////////////////////////
//
//	FILTER FUNCTIONS
//
/////////////////////////////////////

void MMusic::setCutoff(uint16_t c)
{
    cutoff = c;
}


void MMusic::setResonance(uint32_t res)
{
	resonance = res;
    k = res;
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


void MMusic::filterLP6dB() {
	
	int64_t mod = (int64_t(cutoffModAmount) * (int64_t(*cutoffModSource_ptr)))>>16;
	int64_t c = (mod + int64_t(cutoff));
	if(c > 65535) c = 65535;
	else if(c < 0) c = 0;
//	c = ((((c * 32768) >> 15) + 65536) >> 1);

    b1 = filterCoefficient[c>>8];
    a0 = BIT_32 - b1;
    
    sample = (a0 * sample + b1 * lastSampleOutLP) >> 32;
    lastSampleOutLP = sample;

}


void MMusic::filterLP24dB() {  // BROKEN?????
	
	int64_t mod = (int64_t(cutoffModAmount) * (int64_t(*cutoffModSource_ptr)))>>16;
	int64_t c = (mod + int64_t(cutoff));
	if(c > 65535) c = 65535;
	else if(c < 0) c = 0;
    
    b1 = filterCoefficient[c>>8];
    a0 = BIT_32 - b1;
    
    x0 = sample;
    
    y1 = filterSamplesLP24dB[0];
    y2 = filterSamplesLP24dB[1];
    y3 = filterSamplesLP24dB[2];
    y4 = filterSamplesLP24dB[3];
    
    y1 = (a0 * x0 + b1 * y1) >> 32;
    y2 = (a0 * y1 + b1 * y2) >> 32;
    y3 = (a0 * y2 + b1 * y3) >> 32;
    y4 = (a0 * y3 + b1 * y4) >> 32;
    
    filterSamplesLP24dB[0] = y1;
    filterSamplesLP24dB[1] = y2;
    filterSamplesLP24dB[2] = y3;
    filterSamplesLP24dB[3] = y4;
    
    sample = y4;
}



void MMusic::filterHP24dB() {

    int64_t mod = (int64_t(cutoffModAmount) * (int64_t(*cutoffModSource_ptr)))>>16;
    int64_t c = (mod + int64_t(cutoff));
    if(c > 65535) c = 65535;
    else if(c < 0) c = 0;

    b1 = filterCoefficient[c>>8];
    a0 = (BIT_32 + b1) >> 1;
    a1 = -a0;

    xNew = sample;
    xOld = filterSamplesHP24dB[0];
    yOld = filterSamplesHP24dB[4];
    yNew = (a0 * xNew + a1 * xOld + b1 * yOld) >> 32;
    x1 = xNew;
    y1 = yNew;
    
    xNew = y1;
    xOld = filterSamplesHP24dB[1];
    yOld = filterSamplesHP24dB[5];
    yNew = (a0 * xNew + a1 * xOld + b1 * yOld) >> 32;
    x2 = xNew;
    y2 = yNew;

    xNew = y2;
    xOld = filterSamplesHP24dB[2];
    yOld = filterSamplesHP24dB[6];
    yNew = (a0 * xNew + a1 * xOld + b1 * yOld) >> 32;
    x3 = xNew;
    y3 = yNew;
    
    xNew = y3;
    xOld = filterSamplesHP24dB[3];
    yOld = filterSamplesHP24dB[7];
    yNew = (a0 * xNew + a1 * xOld + b1 * yOld) >> 32;
    x4 = xNew;
    y4 = yNew;

    filterSamplesHP24dB[0] = x1;
    filterSamplesHP24dB[1] = x2;
    filterSamplesHP24dB[2] = x3;
    filterSamplesHP24dB[3] = x4;
    
    filterSamplesHP24dB[4] = y1;
    filterSamplesHP24dB[5] = y2;
    filterSamplesHP24dB[6] = y3;
    filterSamplesHP24dB[7] = y4;

    sample = y4;

}


void MMusic::filterMoogLadder() {
    
    int64_t mod = (int64_t(cutoffModAmount) * (int64_t(*cutoffModSource_ptr)))>>16;
	int64_t c = (mod + int64_t(cutoff));
	if(c > 65535) c = 65535;
	else if(c < 0) c = 0;

    int fc = c>>8;
    if(fc > 234) fc = 234;
    x0 = sample;
    u = x0;
    Gstage = filterCoefficientsMoogLadder[1024 + fc];
    
    v1 = ((u - z1) * Gstage) >> 32;
    y1 = (v1 + z1);
    z1 = y1 + v1;
    
    v2 = ((y1 - z2) * Gstage) >> 32;
    y2 = (v2 + z2);
    z2 = y2 + v2;
    
    v3 = ((y2 - z3) * Gstage) >> 32;
    y3 = (v3 + z3);
    z3 = y3 + v3;
    
    v4 = ((y3 - z4) * Gstage) >> 32;
    y4 = (v4 + z4);
    z4 = y4 + v4;
    
    sample = y4;
    
}

void MMusic::filterHP6dB() {
    
    sampleInHP = sample;
	
	int64_t mod = (int64_t(cutoffModAmount) * (int64_t(*cutoffModSource_ptr)))>>16;
	int64_t c = (mod + int64_t(cutoff));
	if(c > 65535) c = 65535;
	else if(c < 0) c = 0;
    //	c = ((((c * 32768) >> 15) + 65536) >> 1);
    
    b1 = filterCoefficient[c>>8];
    a0 = (BIT_32 + b1) >> 1;
    a1 = -a0;
    
    sampleOutHP = (a0 * sampleInHP + a1 * lastSampleInHP + b1 * lastSampleOutHP) >> 32;

    lastSampleInHP = sampleInHP;
    lastSampleOutHP = sampleOutHP;
    sample = sampleOutHP;
    
}



void MMusic::setFilterType(uint8_t type) {
    
    switch (type) {
        case LP6:
            lowpass = true;
            highpass = false;
            lowpass24dB = false;
            highpass24dB = false;
            moogLadder = false;
            break;
        case HP6:
            lowpass = false;
            highpass = true;
            lowpass24dB = false;
            highpass24dB = false;
            moogLadder = false;
            break;
        case BP6:
            lowpass = true;
            highpass = true;
            lowpass24dB = false;
            highpass24dB = false;
            moogLadder = false;
            break;
        case THRU:
            lowpass = false;
            highpass = false;
            lowpass24dB = false;
            highpass24dB = false;
            moogLadder = false;
            break;
        case LP24:
            lowpass = false;
            highpass = false;
            lowpass24dB = true;
            highpass24dB = false;
            moogLadder = false;
            break;
        case HP24:
            lowpass = false;
            highpass = false;
            lowpass24dB = false;
            highpass24dB = true;
            moogLadder = false;
            break;
        case BP24:
            lowpass = false;
            highpass = false;
            lowpass24dB = true;
            highpass24dB = true;
            moogLadder = false;
            break;
        case MOOG:
            lowpass = false;
            highpass = false;
            lowpass24dB = false;
            highpass24dB = false;
            moogLadder = true;
            break;
        default:
            break;
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
}


void MMusic::setDetune(float detune)
{
	detune1 = 0.0;
	detune2 = detune*0.123456789;
	detune3 = -detune;
	setFrequency2(frequency2);
	setFrequency3(frequency3);
}


void MMusic::setDetune1(float detune)
{
	detune1 = detune;
	setFrequency1(frequency1);
}


void MMusic::setDetune2(float detune)
{
	detune2 = detune;
	setFrequency2(frequency2);
}


void MMusic::setDetune3(float detune)
{
	detune3 = detune;
	setFrequency3(frequency3);
}


void MMusic::pitchBend(float b)
{
	bend = b;
	setFrequency1(frequency1);
	setFrequency2(frequency2);
	setFrequency3(frequency3);
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
}


void MMusic::setFM3(uint8_t fm) {
	fmAmount3 = fm;
}


void MMusic::setFMoctaves(uint8_t octs) {
	fmOctaves1 = octs;
	fmOctaves2 = octs;
	fmOctaves3 = octs;
}


void MMusic::setFM1octaves(uint8_t octs) {
	if(octs < 1) octs = 1;
	fmOctaves1 = octs;
}


void MMusic::setFM2octaves(uint8_t octs) {
	if(octs < 1) octs = 1;
	fmOctaves2 = octs;
}


void MMusic::setFM3octaves(uint8_t octs) {
	if(octs < 1) octs = 1;
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
}

void MMusic::noteOnSample(uint8_t note)
{
    int sample = note;
    if(sample < 0) sample = 0;
    if(sample > NUM_SAMPLES) sample = NUM_SAMPLES - 1;
    samplePosition[sample] = 0;
    samplePlaying[sample] = true;
}


void MMusic::noteOff(uint8_t note)
{	
	if(notePlayed = note) {
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

void MMidi::init()
{
	pinMode(0, INPUT);
    pinMode(1, OUTPUT);
//    Serial.begin(9600);
//    MIDI_SERIAL.begin(9600);
    Serial.begin(115200);
    MIDI_SERIAL.begin(31250);
    
    setMidiIn(true);
    setMidiOut(true);
    setMidiThru(true);
    setMidiClockIn(true);
    setMidiClockThru(true);
    setMidiClockOut(true);
    
	midiBufferIndex = 0;
	midiChannel = 0;
    midiRead = false;
    Serial.println("MIDI intialised on channel 1. Use Midi.setChannel(channel) to set to other channel");
}


void MMidi::setChannel(uint8_t channel)
{
    if(channel < 1 || channel > 16) {
        Serial.println("MIDI channel must be set to a number between 1 and 16");
    }
    else midiChannel = channel - 1;
}

uint8_t MMidi::getChannel() {
    return midiChannel;
}


void MMidi::sendNoteOff(uint8_t channel, uint8_t note) {
    if(getMidiOut()) {
        MIDI_SERIAL.write(0x80 | (channel & 0x0F));
        MIDI_SERIAL.write(byte(note & 0x7F));
        MIDI_SERIAL.write(0x00);
    }
}


void MMidi::sendNoteOff(uint8_t channel, uint8_t note, uint8_t vel) {
    if(getMidiOut()) {
        MIDI_SERIAL.write(0x80 | (channel & 0x0F));
        MIDI_SERIAL.write(byte(note & 0x7F));
        MIDI_SERIAL.write(byte(vel & 0x7F));
    }
}


void MMidi::sendNoteOn(uint8_t channel, uint8_t note, uint8_t vel) {
    if(getMidiOut()) {
        MIDI_SERIAL.write(0x90 | (channel & 0x0F));
        MIDI_SERIAL.write(byte(note & 0x7F));
        MIDI_SERIAL.write(byte(vel & 0x7F));
    }
}


void MMidi::sendController(uint8_t channel, uint8_t number, uint8_t value) {
    if(getMidiOut()) {
        MIDI_SERIAL.write(0xB0 | (channel & 0x0F));
        MIDI_SERIAL.write(byte(number & 0x7F));
        MIDI_SERIAL.write(byte(value & 0x7F));
    }
}


void MMidi::sendClock() {
    if(getMidiClockOut()) {
        MIDI_SERIAL.write(MIDI_CLOCK);
    }
}


void MMidi::sendStart() {
    if(getMidiClockOut()) {
        MIDI_SERIAL.write(MIDI_START);
    }
}


void MMidi::sendContinue() {
    if(getMidiClockOut()) {
        MIDI_SERIAL.write(MIDI_CONTINUE);
    }
}


void MMidi::sendStop() {
    if(getMidiClockOut()) {
        MIDI_SERIAL.write(MIDI_STOP);
    }
}


void MMidi::setMidiIn(bool i)
{
    midiIn = i;
}


bool MMidi::getMidiIn()
{
    return midiIn;
}


void MMidi::setMidiOut(bool o)
{
    midiOut = o;
}


bool MMidi::getMidiOut()
{
    return midiOut;
}


void MMidi::setMidiThru(bool t)
{
    midiThru = t;
}


bool MMidi::getMidiThru()
{
    return midiThru;
}


void MMidi::setMidiClockIn(bool i)
{
    midiClockIn = i;
}


bool MMidi::getMidiClockIn()
{
    return midiClockIn;
}


void MMidi::setMidiClockOut(bool o)
{
    midiClockOut = o;
}


bool MMidi::getMidiClockOut()
{
    return midiClockOut;
}


void MMidi::setMidiClockThru(bool t)
{
    midiClockThru = t;
}


bool MMidi::getMidiClockThru()
{
    return midiClockThru;
}


void MMidi::checkSerialMidi()
{
    while(MIDI_SERIAL.available()) {
        
		data = MIDI_SERIAL.read();
		
        if(data >= 0xF8) {
            midiRealTimeHandler(data);
//            RealTimeSystem(byte(data));
            continue;
        }
        
//		if(data & 0x80 && (data & 0x0F) == midiChannel) {	// bitmask with 10000000 to see if byte is over 127 (data&0x80)
//			midiBufferIndex = 0;							// and check if the midi channel corresponds to the midiChannel
//			midiRead = true;								// the device is set to listen to.
//		} else if(data & 0x80) {							// Else if the byte is over 127 (but not on the device's
//			midiRead = false;								// midiChannel, don't read this or any following bytes.
//		}
		
        if(data >= 0x80 && data < 0xF0) {       // check if incoming byte is a status byte (above 127)but less than sysEx (0xF0)
            if((data & 0x0F) == midiChannel) {  // if it is, check if it is the right MIDI channel
                midiBufferIndex = 0;
                midiRead = true;
            } else if(data >= 0x80) {           // if above check fails, check if it is still a status byte
                midiRead = false;
            } else {}                           // if it is below 128 just continue
        }
        
        if(midiRead) {
			midiBuffer[midiBufferIndex] = data;
			midiBufferIndex++;
			if (midiBufferIndex > 2) {
				midiRead = false;
				midiHandler();
//                Serial.println("MIDI RECEIVED");
//                Serial.println(midiBuffer[0], HEX);
//                Serial.println(midiBuffer[1], HEX);
//                Serial.println(midiBuffer[2], HEX);
			}
		}
	}	
}


void MMidi::midiRealTimeHandler(uint8_t data) {
    
    if(getMidiClockThru()) {
        MIDI_SERIAL.write(data);
    }
    if(getMidiClockIn()) {
        switch(data) {
            case 0xF8:
                clock();
                break;
                
            case 0xFA:
                start();
                break;
                
            case 0xFB:
                continues();
                break;
                
            case 0xFC:
                stop();
                break;
                
            default:
                break;
        }

    }
}


void MMidi::clock()
{
        Sequencer.clock();
}


void MMidi::start()
{
        Sequencer.start();
}


void MMidi::continues()
{
        Sequencer.continues();
}


void MMidi::stop()
{
        Sequencer.stop();
}


void MMidi::midiHandler() {

    if(getMidiThru()) {
        MIDI_SERIAL.write(midiBuffer[0]);
        MIDI_SERIAL.write(midiBuffer[1]);
        MIDI_SERIAL.write(midiBuffer[2]);
    }
    if(getMidiIn()) {
        if((midiBuffer[0] & 0x0F) == midiChannel) {
            switch(midiBuffer[0] & 0xF0) { // bit mask with &0xF0
                case 0x80:
                    noteOff			(midiBuffer[0] & 0x0F,     // midi channel 0-15
                                     midiBuffer[1] & 0x7F,   // note value 0-127
                                     midiBuffer[2] & 0x7F);  // note velocity 0-127
                    break;
                    
                case 0x90:
                    noteOn			(midiBuffer[0] & 0x0F,     // midi channel 0-15
                                     midiBuffer[1] & 0x7F,   // note value 0-127
                                     midiBuffer[2] & 0x7F);  // note velocity 0-127
                    break;
                    
                case 0xA0:
                    aftertouch		(midiBuffer[0] & 0x0F,   // midi channel 0-15
                                     midiBuffer[1] & 0x7F, // note value 0-127
                                     midiBuffer[2] & 0x7F);// note velocity 0-127
                    break;
                    
                case 0xB0:
                    controller		(midiBuffer[0] & 0x0F,   // midi channel 0-15
                                     midiBuffer[1] & 0x7F, // controller number 0-127
                                     midiBuffer[2] & 0x7F);// controller value 0-127
                    break;
                    
                case 0xC0:
                    programChange	(midiBuffer[0]  & 0x0F,    // midi channel 0-15
                                     midiBuffer[1] & 0x7F);  // program number 0-127
                    break;
                    
                case 0xD0:
                    channelPressure	(midiBuffer[0]  & 0x0F,    // midi channel 0-15
                                     midiBuffer[1] & 0x7F);  // pressure amount 0-127
                    break;
                    
                case 0xE0:
                    pitchWheel		(midiBuffer[0] & 0x0F,   // midi channel 0-15
                                     midiBuffer[1] & 0x7F, // higher bits 0-6
                                     midiBuffer[2] & 0x7F);// lower bits 7-13
                    break;
                    
                default:
                    break;
            }
        }
        else Serial.println("Skipped MIDI message on other channel");
    }
}


void MMidi::noteOff(uint8_t channel, uint8_t note, uint8_t vel) {
//    Serial.print("NoteOff received on channel: ");
//    Serial.println(channel + 1, HEX);

    Music.noteOff(note);
}


void MMidi::noteOn(uint8_t channel, uint8_t note, uint8_t vel) {
//    Serial.print("NoteOn received on channel: ");
//    Serial.println(channel + 1, HEX);
    Music.noteOn(note, vel);
}


void MMidi::aftertouch(uint8_t channel, uint8_t note, uint8_t pressure) {
	// Write code here for Aftertouch 
}


void MMidi::controller(uint8_t channel, uint8_t number, uint8_t value) {
	
	if(value > 127) value = 127;
	instrument[number] = value;
	
	switch(number) {
		case IS_12_BIT:
			if(value) Music.set12bit(true);
			else Music.set12bit(false);
			break;
		case PORTAMENTO:
			Music.setPortamento(portamentoTimeTable[value]);
			break;
		case CUTOFF:
			Music.setCutoff(value * 512);
			break;			
		case RESONANCE:
			Music.setResonance(value * 512);
			break;			
		case FILTER_TYPE:
			Music.setFilterType(value);
			break;
		case CUTOFF_MOD_AMOUNT:
			Music.setCutoffModAmount((value-64) * 1024);
			break;
		case CUTOFF_SOURCE:
			Music.setCutoffModSource(value);
			break;			
		case CUTOFF_SHAPE:
			Music.setCutoffModShape(value);
			break;			
		case ZERO_HZ_FM:
			if(value) Music.fmToZeroHertz(true);
			else Music.fmToZeroHertz(false);
			break;
		case FM_OCTAVES:
			Music.setFMoctaves(value+1);
			break;
		case ENVELOPE_MULTIPLIER:
		    envelopeMultiplier = value;
		    envelopeMultiplier = constrain(envelopeMultiplier,1,127);
		    break;
		case LFO1:
			if(value) {
				Music.setOsc1LFO(true);
				Music.setFrequency1(Music.getNoteFrequency(value)/1024.0);
			} else {
				Music.setOsc1LFO(false);
			}
			break;
		case LFO2:
			if(value) {
				Music.setOsc2LFO(true);
				Music.setFrequency2(Music.getNoteFrequency(value)/1024.0);
			} else {
				Music.setOsc2LFO(false);
			}
			break;
		case LFO3:
			if(value) {
				Music.setOsc3LFO(true);
				Music.setFrequency3(Music.getNoteFrequency(value)/1024.0);
			} else {
				Music.setOsc3LFO(false);
			}
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
//			if(15 < value && value < 113) {
//				int8_t val = (((value-16)/2)-24);
//				Music.setSemitone1(val);
//			} else if (value < 16) {
//				Music.setSemitone1(-24);				
//			} else {
//				Music.setSemitone1(24);
//			}
			if(40 <= value && value <= 88) {
				Music.setSemitone1(value-64);
			} else if (value < 40) {
				Music.setSemitone1(-24);				
			} else {
				Music.setSemitone1(24);
			}
			break;
		case SEMITONE2:
//			if(15 < value && value < 113) {
//				int8_t val = (((value-16)/2)-24);
//				Music.setSemitone2(val);
//			} else if (value < 16) {
//				Music.setSemitone2(-24);				
//			} else {
//				Music.setSemitone2(24);
//			}
			if(40 <= value && value <= 88) {
				Music.setSemitone2(value-64);
			} else if (value < 40) {
				Music.setSemitone2(-24);				
			} else {
				Music.setSemitone2(24);
			}
			break;
		case SEMITONE3:
//			if(15 < value && value < 113) {
//				int8_t val = (((value-16)/2)-24);
//				Music.setSemitone3(val);
//			} else if (value < 16) {
//				Music.setSemitone3(-24);				
//			} else {
//				Music.setSemitone3(24);
//			}
			if(40 <= value && value <= 88) {
				Music.setSemitone3(value-64);
			} else if (value < 40) {
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
			Music.setWaveform1(value);
			break;
		case WAVEFORM2:
			Music.setWaveform2(value);
			break;
		case WAVEFORM3:
			Music.setWaveform3(value);
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
//		case FM1_OCTAVES:
//			Music.setFM1octaves(value+1);
//			break;
//		case FM2_OCTAVES:
//			Music.setFM2octaves(value+1);
//			break;
//		case FM3_OCTAVES:
//			Music.setFM3octaves(value+1);
//			break;
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
			if(value) Music.enableEnvelope1();
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
			if(value) Music.enableEnvelope2();
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
		case PRESET_SAVE:
			Music.savePreset(value);
			break;
		case PRESET_RECALL:
			Music.getPreset(value);
			Music.sendInstrument();
			break;
        case SEQ_INTERNAL_CLOCK:
//            Sequencer.setInternalClock(value);
            break;
        case SEQ_CLOCK_IN:
//            setMidiClockIn(value);
            break;
        case SEQ_CLOCK_THRU:
//            setMidiClockThru(value);
            break;
        case SEQ_CLOCK_OUT:
//            setMidiClockOut(value);
            break;
        case SEQ_BPM:
//            Sequencer.setbpm(60 + value);
            break;
        case SEQ_SEQUENCE:
//            Sequencer.setSelectedSequence(value);
            break;
        default:
			break;
	} 
}


void MMidi::programChange(uint8_t channel, uint8_t number) {
//	Music.getPreset(number);
}


void MMidi::channelPressure(uint8_t channel, uint8_t pressure) {
	// Write code here for Channel Pressure 
}


void MMidi::pitchWheel(uint8_t channel, uint8_t highBits, uint8_t lowBits) {
	// Write code here for Pitch Wheel
}

