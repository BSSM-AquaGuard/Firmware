#pragma once
#include <OneWire.h>
#include <DS18B20.h>
class Temperature{
    public:
        Temperature();
        float getTemperatureC();
    private:
        OneWire _wire;
        DS18B20 _sensor;
};