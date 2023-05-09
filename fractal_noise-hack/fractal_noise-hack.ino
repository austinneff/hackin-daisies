
#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc;

float pitchknob;

FractalRandomGenerator<ClockedNoise, 5> fract;
Oscillator lfo[2];

void AudioCallback(float **in, float **out, size_t size) {
    osc.SetFreq(mtof(pitchknob));
  for (size_t i = 0; i < size; i++) {
    float sig = osc.Process();

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
  for (size_t i = 0; i < size; i++) {
    fract.SetFreq(fabsf(lfo[0].Process()));
    fract.SetColor(fabsf(lfo[1].Process()));
    out[0][i] = out[1][i] = fract.Process();
  }
}


void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);
  osc.SetFreq(440);
  osc.SetAmp(0.5);
  osc.SetWaveform(osc.WAVE_TRI);

  fract.Init(sample_rate);
  fract.SetFreq(sample_rate / 10.f);

  lfo[0].Init(sample_rate);
  lfo[1].Init(sample_rate);

  lfo[0].SetFreq(.25f);
  lfo[0].SetAmp(sample_rate / 4.f);
  lfo[1].SetFreq(.1f);
  lfo[1].SetAmp(1.f);

  osc.Init(sample_rate);
  osc.SetFreq(440);
  osc.SetAmp(0.5);
  osc.SetWaveform(osc.WAVE_TRI);


  DAISY.begin(AudioCallback);
}

void loop() { pitchknob = 24.0 + ((analogRead(A0) / 1023.0) * 60.0); }
