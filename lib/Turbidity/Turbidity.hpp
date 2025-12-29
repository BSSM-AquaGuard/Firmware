#pragma once
#include <stdint.h>

class Turbidity{
    public:
        Turbidity(uint8_t pin);
        float get();
    private:
        uint8_t _pin;
};