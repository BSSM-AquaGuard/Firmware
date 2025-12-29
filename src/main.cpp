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
  timestamp(&now); 
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
    now.year, now.month, now.day,
    now.hour, now.minute, now.second);

  int sensorValue = analogRead(TURBIDITY); 

  float voltage = sensorValue * (5.0 / 1024.0); 

  Serial.println(voltage);

  delay(500);

} 