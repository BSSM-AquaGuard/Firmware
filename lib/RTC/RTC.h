#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#define SDA_PIN 21
#define SCL_PIN 22

#include <Arduino.h>

typedef struct{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} RTC_Time;

void getTimestamp(RTC_Time *now);
String getTimestampToString();
void rtc_init();

#endif