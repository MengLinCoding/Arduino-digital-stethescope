#include "Arduino.h"
#include "Wavelet.h"

// Collects microphone analogue data. Set baud rate to maximum possible value
const unsigned long baudrate = 1000000;

// Signal detection and ground pins 
const unsigned char pinSignal = 15;   // ESP32

// Desired time taken to collect new data (microseconds). 
// Arduino: 4kHz (250us), ESP32: 20kHz, (25us)
// 2kHz (500us)
// 3.125kHz (320us)
// 4kHz (250us)
// 5kHz (200us)
// 10kHz (100us)
// 20kHz (50us)
  
// note: esp32 max sample rate is 6kHz
const unsigned long sampleTime = 500;
const unsigned char subSample = 5;
const float sampleFreq = pow(10,6)/sampleTime;

// to pass value between cores
QueueHandle_t queueValue;









void core0( void * pvParameters ){
  float WaveletFreq = 100;
  int windowSize = round(6*sampleFreq/WaveletFreq);
  Wavelet Wavefft = Wavelet( WaveletFreq , pow(10,6)/sampleTime , windowSize );
  float dataBuffer[windowSize] = {0};
  float receiver = 0;
  float receiverOld = 0;
  float receiverOldest = 0;
  
  float thresholdLow = 3;
  float heightAve = thresholdLow;
  float thresholdHigh = 10000000;

  const int bufferLen = 10;
  float peakTimeHistory[bufferLen] = {0};
  //float peakHeightHistory[bufferLen] = {0};
  int peakTime;
  float heartRate;

  for(;;){
    receiverOldest = receiverOld;
    receiverOld = receiver;
    xQueueReceive(queueValue, &receiver, portMAX_DELAY);
    pushPop(receiver, dataBuffer, windowSize);
    receiver = Wavefft.coefSquared(dataBuffer);

    // heightOld is the peak in this case
    if( receiverOld > thresholdLow && thresholdHigh > receiverOld && receiver < receiverOld && receiverOld > receiverOldest ){
      const int len = 5;
      float varTime = variance(peakTimeHistory, len);
      float aveTime = mean(peakTimeHistory, len);
      if( (peakTimeHistory[0]*0.85 < peakTime  && peakTimeHistory[bufferLen-1] == 0) || varTime < pow(aveTime*0.2,2) && pow(aveTime*0.2,2) > pow(aveTime-peakTime, 2) ){
        pushPop(peakTime, peakTimeHistory, bufferLen);
        heartRate = samples2heartRate( peakTimeHistory[0] , sampleTime*pow(10,-6) );
        peakTime = 0;
        if( peakTimeHistory[1] > 0 ){
          heightAve = 0.5*heightAve + 0.5*receiverOld;
          thresholdLow = (0.5*heightAve + 0.5*receiverOld)*0.3;
          thresholdHigh = (0.5*heightAve + 0.5*receiverOld)*3.0;
        }
      }
      //Serial.println( receiverOld );
      if( varTime < pow(aveTime*0.2,2) && aveTime*0.85 < peakTime ){
        peakTime = 0;
      }
      
    } else {
      peakTime++;
    }

    Serial.print( heartRate );
    Serial.print(",");
    Serial.print( thresholdLow );
    Serial.print(",");
    Serial.print( thresholdHigh );
    Serial.print(",");
    Serial.println(receiver);
    Serial.flush();
    delayMicroseconds(80);
  }
}











void setup(){
  pinMode(pinSignal, INPUT);
  // set to highest baud rate possible
  Serial.begin(baudrate);  

  queueValue = xQueueCreate( 20, sizeof( float ) );
  xTaskCreatePinnedToCore(core0, "core0", 10000, NULL, 0, NULL, 0);
  delay(1);
}










void loop(){
  long temp = 0;      // Helps to keep track if the arduino collecting data on time.
  float valueNew = 0; // Used to identify the value of the signal
  unsigned char counter = 0;

  // Send average of subsampled data to 'core0'
  for(;;){
    temp = delayPeriod_us(floor(sampleTime/subSample));
    valueNew += analogRead(pinSignal);
    counter++;
    if(counter == subSample){
      counter = 0;
      float messenger = valueNew * (1.0/subSample);
      xQueueSend(queueValue, &messenger, 1);
      valueNew = 0;
    }
  }
}
