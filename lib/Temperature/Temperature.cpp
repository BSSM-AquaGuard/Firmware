#include "Temperature.hpp"

Temperature::Temperature(OneWire wire): _wire(wire), _sensor(&_wire){
    _sensor.begin();
}
float Temperature::getTemperatureC(){
    _sensor.requestTemperatures();
    return _sensor.getTempC();
}