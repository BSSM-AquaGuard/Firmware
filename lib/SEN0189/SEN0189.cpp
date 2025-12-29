#include "SEN0189.hpp"
#include <Arduino.h>

SEN0189::SEN0189(uint8_t pin): _pin(pin){}

float SEN0189::readPPM(){
    int sensorValue = analogRead(_pin); 
    float voltage = sensorValue * (5.0 / 1024.0); 
    return voltage;
}