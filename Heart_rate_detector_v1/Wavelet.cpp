#include "Wavelet.h"

Wavelet::Wavelet(float freq, float freqData, int windowSize)
{
  float freqRelative = freq/freqData;
  this->windowSize = windowSize;
  this->freq = freq;
  this->freqData = freqData;
      
  sinWavelet = (float*) malloc(windowSize * sizeof(float));
  cosWavelet = (float*) malloc(windowSize * sizeof(float));
  float normalise = 1.0/windowSize;
  for(int i = 0; i<windowSize; i++){
     sinWavelet[i] = sin(2*PI*i*freqRelative)*normalise;
     cosWavelet[i] = cos(2*PI*i*freqRelative)*normalise;

     // Hann window
     //sinWavelet[i] *= 0.5*(1-cos(2*PI*i/windowSize));
     //cosWavelet[i] *= 0.5*(1-cos(2*PI*i/windowSize));

     // Blackman-Harris window
     sinWavelet[i] *= 0.35875-0.48829*cos(2*PI*i/windowSize)+0.14128*cos(4*PI*i/windowSize)-0.01168*cos(6*PI*i/windowSize);
     cosWavelet[i] *= 0.35875-0.48829*cos(2*PI*i/windowSize)+0.14128*cos(4*PI*i/windowSize)-0.01168*cos(6*PI*i/windowSize);
  }
}
float Wavelet::coefSquared(float *input){
  float sinIntergral = 0;
  float cosIntergral = 0;

  // trapezoidal integration for wavelet matched filter for sin/cos components
  sinIntergral += 0.5*( input[0]*sinWavelet[0] + input[windowSize-1]*sinWavelet[windowSize-1] );
  cosIntergral += 0.5*( input[0]*cosWavelet[0] + input[windowSize-1]*cosWavelet[windowSize-1] );
  for(int i = 1; i<windowSize-1; i++){
    sinIntergral += input[i]*sinWavelet[i];
    cosIntergral += input[i]*cosWavelet[i];
  }
  return pow(sinIntergral,2) + pow(cosIntergral,2);
}
