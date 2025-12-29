#include <Wire.h>
#include <RTClib.h>
#include "RTC.h"
#define TURBIDITY 34

void setup () {

  Serial.begin(115200);
  rtc_init();
 
}

void loop(){

  RTC_Time now;
  getTimestamp(&now); 
  Serial.printf(getTimestampToString());

  int sensorValue = analogRead(TURBIDITY); 

  float voltage = sensorValue * (5.0 / 1024.0); 

  Serial.println(voltage);

  delay(500);
} 