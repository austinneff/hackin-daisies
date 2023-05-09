// Title: adenv
// Description: Triangle wave controlled by AD envelope
// Hardware: Daisy Seed
// Author: Stephen Hensley

#include "DaisyDuino.h"

size_t num_channels;

DaisyHardware hw;

static Metro tick;
static AdEnv adenv;
static Oscillator osc;
static Svf filter;

static Parameter filter_cutoff;

float decayknob = 0;

void MyCallback(float **in, float **out, size_t size) {
  adenv.SetTime(ADENV_SEG_DECAY, decayknob);
  for (size_t i = 0; i < size; i++) {
    if (tick.Process()) {
      adenv.Trigger();
    }

    float env_out = adenv.Process();
    float sig = osc.Process();
    sig *= env_out;

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
  tick.Init(1, sample_rate);
  osc.Init(sample_rate);
  filter.Init(sample_rate);

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

  DAISY.begin(MyCallback);
}

void loop() {
  decayknob = analogRead(A1) / 1023.0;
}
