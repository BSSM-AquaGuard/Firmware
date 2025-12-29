#include "Temperature.hpp"

Temperature::Temperature(): _wire(4), _sensor(&_wire){
    _sensor.begin();
}

float Temperature::getTemperatureC(){
    _sensor.requestTemperatures();
    return _sensor.getTempC();
}