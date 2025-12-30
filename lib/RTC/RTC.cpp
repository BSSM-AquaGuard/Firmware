#include "RTC.hpp"
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;
bool rtcAvailable = false;

void rtc_init(){
    if (!rtc.begin()) {
        Serial.println("[RTC] Failed to find RTC module!");
        rtcAvailable = false;
        return;
    }
    
    Serial.println("[RTC] RTC module found!");
    
    if (!rtc.isrunning()) {
        Serial.println("[RTC] RTC is NOT running, setting time...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        Serial.println("[RTC] RTC is already running");
    }
    
    rtcAvailable = true;
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

uint32_t getTimestampToUnix(){
    DateTime current = rtc.now();
    return current.unixtime();
}