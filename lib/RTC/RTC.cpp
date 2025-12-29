#include "RTC.h"
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

void rtc_init(){
    Wire.begin(SDA_PIN, SCL_PIN);
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void timestamp(RTC_Time* now){
    DateTime current = rtc.now();

    now -> year = current.year();
    now -> month = current.month();
    now -> day = current.day();
    now -> hour = current.hour();
    now -> minute = current.minute();
    now -> second = current.second();
}