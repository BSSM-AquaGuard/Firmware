#include "RTC.hpp"
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

void rtc_init(){
    Wire.begin(SDA_PIN, SCL_PIN);
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void getTimestamp(RTC_Time* now){
    DateTime current = rtc.now();

    now -> year = current.year();
    now -> month = current.month();
    now -> day = current.day();
    now -> hour = current.hour();
    now -> minute = current.minute();
    now -> second = current.second();
}

String getTimestampToString(){
    DateTime current = rtc.now();
    char buffer[20];
    sprintf(buffer, "%04d/%02d/%02d %02d:%02d:%02d",
        current.year(), current.month(), current.day(),
        current.hour(), current.minute(), current.second());
    return String(buffer);
}