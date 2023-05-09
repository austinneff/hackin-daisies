
// Title: oscillator
// Description: Control a sine wave freq with a knob
// Hardware: Daisy Seed
// Author: Ben Sergentanis
// Breadboard
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Osc/resources/Osc_bb.png
// Schematic:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Osc/resources/Osc_schem.png

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Metro tick;
static AdEnv adenv;
static Oscillator osc;
static Svf filter;
static ReverbSc verb;


float pitchknob, decayknob, filterknob;

void MyCallback(float **in, float **out, size_t size) {
  float verb_out_l, verb_out_r;

  // Convert Pitchknob MIDI Note Number to frequency
  osc.SetFreq(mtof(pitchknob));
  // Set Decay time
  adenv.SetTime(ADENV_SEG_DECAY, decayknob);
  // Set filter cutoff
  filter.SetFreq(filterknob);
  filter.SetRes(decayknob);


  for (size_t i = 0; i < size; i++) {
    //float dryl, dryr, wetl, wetr;
    
    if (tick.Process()) {
      adenv.Trigger();
    }
    // get envelope value
    float env_out = adenv.Process();
    // get oscillator value
    float sig = osc.Process();

    // send the signal through the envelope
    sig = sig * env_out;

    // send the signal through the filter
    filter.Process(sig);

    sig = filter.Low();

    // send the signal through reverb
    verb.Process(sig, sig, &verb_out_l, &verb_out_r);
    sig = (sig + verb_out_l);
    //sig = verb_out_l;


    

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  adenv.Init(sample_rate);
  tick.Init(2.0f, sample_rate);
  osc.Init(sample_rate);
  filter.Init(sample_rate);
  verb.Init(sample_rate);
 

  // Set oscillator parameters
  osc.SetFreq(440);
  osc.SetAmp(0.5);
  osc.SetWaveform(osc.WAVE_TRI);

  // Set envelope parameters
  adenv.SetTime(ADENV_SEG_ATTACK, 0.15);
  adenv.SetTime(ADENV_SEG_DECAY, 0.35);
  adenv.SetMin(0.0);
  adenv.SetMax(0.25);
  adenv.SetCurve(0); // linear

  // Set filter parameters
  filter.SetFreq(500.0);
  filter.SetRes(0.85);
  filter.SetDrive(0.8);

  // Set reverb parameters
  verb.SetFeedback(1.0f);
  verb.SetLpFreq(2000.0f);



  DAISY.begin(MyCallback);

  Serial.begin(9600);
}

void loop() { 
  pitchknob = 24.0 + ((analogRead(A0) / 1023.0) * 60.0); 
  decayknob = analogRead(A1) / 1023.0;
  filterknob = ((analogRead(A2) / 1023.0) * 10000.0);
  Serial.println(filterknob);
}
