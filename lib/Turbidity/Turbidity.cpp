#include "Turbidity.hpp"
#include <Arduino.h>

Turbidity::Turbidity(uint8_t pin): _pin(pin){}

float Turbidity::get(){
    int sensorValue = analogRead(_pin); 
    float voltage = sensorValue * (5.0 / 1024.0); 
    return voltage;
}