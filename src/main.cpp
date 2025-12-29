#include <Arduino.h>
#include <Temperature.hpp>
#include <Wire.h>
#include <RTClib.h>
#include "RTC.h"
#define TURBIDITY 34

Temperature Sensor(4);

void setup() {
  Serial.begin(115200);
  rtc_init();
}

void loop() {
  float temp = Sensor.getTemperatureC();
  Serial.println(temp);
  RTC_Time now;
  getTimestamp(&now); 
  Serial.printf(getTimestampToString());

  int sensorValue = analogRead(TURBIDITY); 

  float voltage = sensorValue * (5.0 / 1024.0); 

  Serial.println(voltage);

  delay(500);
}
