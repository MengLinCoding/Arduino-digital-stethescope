// Collects microphone analogue data. Set baud rate to maximum possible value
const unsigned long baudrate = 1000000;

// Signal detection and ground pins 
//const unsigned char pinSignal = A0;  // Arduino Uno
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
const unsigned long subSample = 5;
long temp = 0;

// Used to identify the value of 
float valueNew = 1000;
float mean = 0;
float meanOld = 3000;
float variance = 0;
float varianceLow = 1000;
float varianceOld = 0;


unsigned char counter = 0;

QueueHandle_t queueValue;



//* 
void core0( void * pvParameters ){
  for(;;){
    //Serial.println(xPortGetCoreID());
    float receiver;
    xQueueReceive(queueValue, &receiver, portMAX_DELAY);
    Serial.println(receiver);
    //Serial.println(timeStep());
    Serial.flush();
    delayMicroseconds(80);
  }
}
//*/




void setup(){
  pinMode(pinSignal, INPUT);
  // set to highest baud rate possible
  Serial.begin(baudrate);  

  queueValue = xQueueCreate( 20, sizeof( float ) );
  xTaskCreatePinnedToCore(core0, "core0", 10000, NULL, 0, NULL, 0);
}





void loop(){
  temp = delayPeriod_us(sampleTime/subSample);

  valueNew = analogRead(pinSignal);
  mean = valueNew*0.0001 + meanOld*0.9999;
  meanOld = mean;
  variance = pow(valueNew-mean,2);
  varianceLow = variance*0.0001+varianceOld*0.9999;
  varianceOld = varianceLow;
    
  counter++;

  if(counter == subSample){
    counter = 0;
    
    //valueNew = (valueNew-3000)*5;
    //if(valueNew < 1000){
      //valueNew = 1000;  
    //} else if(valueNew > 9999){
      //valueNew = 9999;
    //}
    
    //float messenger = variance;
    //float messenger = timeStep();
    //xQueueSend(queueValue, &messenger, 1);

    /*
    Serial.print( valueNew );  
    Serial.print(",");
    Serial.print( mean );
    Serial.print(",");
    Serial.print( variance );
    Serial.print(",");
    Serial.println( varianceLow );
    //*/
    
    
    //Serial.println( temp );
    // timeStep is used for debugging: if desired sample time is reached
    //Serial.println( timeStep());
    //Serial.println(xPortGetCoreID());
  }
  
}
