#pragma once
#include <stdint.h>

class SEN0189{
    public:
        SEN0189(uint8_t pin);
        float readPPM();
    private:
        uint8_t _pin;
};