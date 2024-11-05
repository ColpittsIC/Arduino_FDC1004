/*
  FDC1004 - basic

  This example reads data from Modulino Farad
  With this settings you can read channel 1.
  Offset and gain are set to Zero.


  This example code is in the public domain.
*/
#include    <Arduino_FDC1004.h>


int measured_cap;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 

  if (!FDC1004.begin()) {
    Serial.println("Failed to initialize FDC1004 Capacitive Sensor!");
    while (1);
  }
  //Offset settings - Value set to Zero
  FDC1004.channelOffset(CHANNEL1,0x00);
  //Channel settings - Channel 1 will measure the capacitance on input CIN1sa and the offset is set to Zero 
  FDC1004.channelConfiguration(CHANNEL1,CIN1,CAPDAC,0x00);
  //Measure settings - Channel 1 enabled, sample rate is 100 Samples/sec and measure is repeated
  FDC1004.measurementConfiguration(MEAS_1_EN,RATE_100Ss,REPEAT_ENABLED);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (FDC1004.measureAvailable(CHANNEL1)) 
  {     
    measured_cap = FDC1004.getChannelMeasurement(CHANNEL1);
    Serial.print("The Value of the measured Capacitance is ");
    Serial.println(measured_cap);
  } 
  delay(200);
}
