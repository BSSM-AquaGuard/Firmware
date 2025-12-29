#include <Wire.h>
#include "ArduinoJson.h"
#include <SEN0189.hpp>
#include <RTClib.h>

RTC_DS1307 RTC;
SEN0189 turbidity(34);

#define SDA_PATH 21
#define SCL_PATH 22
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {

  Serial.begin(115200);

  Wire.begin(SDA_PATH, SCL_PATH);

  RTC.begin();

  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
}

void loop(){
  Serial.println(turbidity.readPPM()); 

  delay(500);
  DateTime now = RTC.now();
  Serial.print(now.year(), DEC);  // 년

  Serial.print('/');

  Serial.print(now.month(), DEC);  // 월

  Serial.print('/');

  Serial.print(now.day(), DEC);   // 일

  Serial.print(' ');

  Serial.print(now.hour(), DEC);    // 시간

  Serial.print(':');

  Serial.print(now.minute(), DEC);   // 분

  Serial.print(':');

  Serial.print(now.second(), DEC);   // 초

  Serial.print(' ');

  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);   // 요일

  Serial.println();

  delay(1000); 
} 