// Records time 
long delayPeriod_us(unsigned long period)
{  
  static unsigned long oldTimeStamp = 0;
  long timeDelay;
  unsigned long newTimeStamp = micros();

  // Check how long to delay for
  timeDelay = period - newTimeStamp + oldTimeStamp;
  if(timeDelay > 0){
    delayMicroseconds( timeDelay - 2);
    oldTimeStamp = micros();
  } else {
    oldTimeStamp = newTimeStamp;
  }
  
  return timeDelay;
}
