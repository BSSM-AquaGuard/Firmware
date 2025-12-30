#include "PH.hpp"
#include <Arduino.h>

#define ADC_MAX 4095.0
#define VREF    3.3

PH::PH(uint8_t pin) : _pin(pin) {
    analogReadResolution(12);   // ESP32
}

void PH::calibrate(float a, float b) {
    _a = a;
    _b = b;
}

float PH::readVoltage() {
    int buf[10];

    for(int i = 0; i < 10; i++) {
        buf[i] = analogRead(_pin);
        delay(10);
    }

    // 정렬
    for(int i = 0; i < 9; i++) {
        for(int j = i + 1; j < 10; j++) {
            if(buf[i] > buf[j]) {
                int t = buf[i];
                buf[i] = buf[j];
                buf[j] = t;
            }
        }
    }

    long sum = 0;
    for(int i = 2; i < 8; i++)
        sum += buf[i];

    float adc = sum / 6.0;
    return adc * VREF / ADC_MAX;
}

float PH::get() {
    float voltage = readVoltage();
    float pH = _a * voltage + _b;
    return pH;
}
