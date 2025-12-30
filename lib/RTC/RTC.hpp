#ifndef RTC_MODULE_H
#define RTC_MODULE_H

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
uint32_t getTimestampToUnix();
String getTimestampToString();
void rtc_init();

#endif