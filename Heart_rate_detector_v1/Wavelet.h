#ifndef WAVELET_H
#define WAVELET_H

#include <Arduino.h>

class Wavelet{
  private:
    int windowSize;
    float *sinWavelet;
    float *cosWavelet;
    float freq;
    float freqData;
  public:
    Wavelet(float freq, float freqData, int windowSize);
    float coefSquared(float *input);
};

#endif
