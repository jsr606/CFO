/////////////////////////////////////////////////////////////////////////
// DON'T TRY TO RUN THIS SKETCH, IT IS FOR DOCUMENTATION PURPOSES ONLY //
/////////////////////////////////////////////////////////////////////////

// These are the music functions that are available for you to use in you sketches.
// You can see examples of how some of them are used (the most basic ones) in the
// Apps folder that downloaded with the MM library.

// In the following you can see the variable type that the function takes as an argument (float, uint8_t or uint16_t)
// The weirdly looking "uint16_t" and "uint8_t" is just unsigned 16 and 8 bit integers. So instead of having
// both negative and positive values, they only have positive values from 0 - 255 (8bit) and 0 - 65535 (16bit).
// If you copy a function from here to your arduino sketch, just change the word "float", "uint8_t", etc into the
// variable name that you uses in your sketch.

// PRE-PROCESSING COMMANDS
#define NUM_OSCILLATORS 3  // define the use of 1, 2 or 3 oscillators
#define BIT_DEPTH 12       // define the bit depth resolution of 8 or 12 bit waveforms
#define MIDI               // tell the system you will be using the MIDI part of the sound engine
#define MIDI_CHANNEL 1     // set the MIDI channel to listen to. Between 1 and 16
#define FM                 // tell the library to use the FM synthesis mode

#include <Music.h>

// INITIALIZERS
// Use this in the setup() function to start the synth engine.
// It defaults to a sine tone at 110Hz, no envelope and no detune.
Music.init();

// Use this in the setup() function to start the MIDI engine. It sets up the serial communication. 
Midi.init();

// MIDI FUNCTIONS
Midi.checkMidi() // put this function in the loop() function in your
                 // Arduino sketch to check for incoming MIDI activity

// FREQUENCY AND DETUNE FUNCTIONS
// Use these functions to set the frequency and detune parameters of the synth.
Music.setFrequency(float);   // Set base frequencies of all oscillators at once. Does _not_ affect detune or semitone
Music.setFrequency1(float);  // Set base frequency of individual oscillators.
Music.setFrequency2(float);  // Sounds best between 20Hz and 4000Hz
Music.setFrequency3(float);  //
Music.setDetune(float);      // Set the detune offset of all oscillators at once. Oscillator 1 stays fixed at it's base frequency
Music.setDetune1(float);     // Set the detune of oscillator 1,2 and 3 individually
Music.setDetune2(float);     // Sounds best between 0.00 and 0.02    
Music.setDetune3(float);     //
Music.setSemitone1(int8_t);  // Set the semitone offset of base frequency in musical halftones relative to base frequency
Music.setSemitone2(int8_t);  // Goes from -24 halftones to +24 halftones
Music.setSemitone3(int8_t);
Music.setFM2(uint8_t);       // Set the amount of frequency modulation (FM) on oscillator2.


// WAVEFORM FUNCTIONS
// Switch between the different waveforms for the oscillators. ONLY FOR 8BIT MODE!
Music.setWaveform(uint16_t);         // This sets waveform of all oscillators at once.
Music.setWaveform1(uint16_t);        // Takes a number from 0-15
Music.setWaveform2(uint16_t);        // Or take one of the macro names (in capitals) below for waveform types.
Music.setWaveform3(uint16_t);
// WAVEFORM TYPES
SINE
SQUARE
PULSE
TRIANGLE
SAW
FUZZ
DIGI1
DIGI2
DIGI3
DIGI4
NOISE
DIGI6
TAN1
TAN2
TAN3
TAN4

// GAIN FUNCTIONS
// Set the gain of the oscillators all at once or individually. Use floats between 0.0 and 1.0
Music.setGain(float); // 0.0 - 1.0
Music.setGain1(float); // 0.0 - 1.0 
Music.setGain2(float); // 0.0 - 1.0
Music.setGain3(float); // 0.0 - 1.0
// You can also ask the sound engine  
Music.getGain(); // outputs a float between 0.0 and 1.0
Music.getGain1(); //  
Music.getGain2(); // 
Music.getGain3(); // 


// NOTE FUNCTIONS
// These functions triggers a note to be played. The noteOff() functions turns the note off again.
// They come both with note and velocity information (for noteOn). If you don't know what that is,
// just use the ones with the least arguments.
// To get a proper note sound call Music.enableEnvelopes() [see below] before calling the noteOn function.
// You just have to do that once in the setup for example.
Music.noteOn(uint8_t, uint8_t);  // 0 - 127, first argument is the note number, the second is the velocity
Music.noteOn(uint8_t);           // 0 - 127, here it is only note number and velocity defaults to 127
Music.noteOff(uint8_t);          // 0 - 127, specify which note should be turned off (not necessary yet)
Music.noteOff();                 // turns off last played note
// This function returns the frequency of a MIDI note number sent to it.
Music.getNoteFrequency(uint8_t); // input 0 - 127, returns a frequency in unsigned 16bit integers


// ENVELOPE FUNCTIONS
// These functions enables and sets the parameters of the internal envelope which creates dynamics for the notes 
// being played. You can read about ADSR envelopes here: http://en.wikipedia.org/wiki/Synthesizer#ADSR_envelope
// When using the envelope you can only hear sound when you are triggering the notes with the note functions. In order
// to get dynamics without triggering the note object you must have the envelope turned off, for example using 
// the Music.disableEnvelope() function [already set by default in the init() function]. You can then control the
// dynamics of the sound with the overall or individual setGain() functions.
Music.enableEnvelope();
Music.disableEnvelope();

// Alternately to using the noteOn/NoteOff functions you can set the envelope stage. Experiment with it :)
Music.setEnvStage(uint8_t); // 0 - 4, where 0 is in "silent" stage, 1 is attack, 2 is decay, 3 is sustain and 4 is release stage.

// Setting the parameters for the envelope you send an 8bit number between 0 and 127 to the functions below. 0 is a very fast
// rise or decay in sound, whereas 127 is very long. Sustain is the sound level where 0 is silent and 127 is full gain. 
// You must experiment with what suits your musical taste :)
// These parameters can of course be adjusted during the physics code for interesting results, but be aware that when
// using the sine wave oscillator (which is more processor intensive) the sound can hang or have glitches if you alter
// these parameters too quickly or set them at extremes. Try it out.
Music.setAttack(uint8_t); // 0 - 127
Music.setDecay(uint8_t); // 0 - 127
Music.setSustain(uint8_t); // 0 - 127
Music.setRelease(uint8_t); // 0 - 127

