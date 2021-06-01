float samples2heartRate(int samples, float samplingTime_s){
  return 60 / (samples * samplingTime_s);
}
