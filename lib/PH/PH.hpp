#pragma once
#include <stdint.h>

class PH{
    public:
        PH(uint8_t pin);
        void calibrate(float a, float b);
        float get();
    private:
        uint8_t _pin;
        float _a;
        float _b;
        float readVoltage();
};