#include "Lora.hpp"
#include <freertos/FreeRTOS.h>

Lora::Lora(HardwareSerial& serial, uint8_t m0, uint8_t m1, uint8_t aux)
    : _serial(serial), _m0(m0), _m1(m1), _aux(aux) {
        
}
void Lora::begin(){
    pinMode(_m0, OUTPUT);
    pinMode(_m1, OUTPUT);
    pinMode(_aux, INPUT_PULLUP);
}
void Lora::setMode(Mode mode) {
    switch (mode) {
        case MODE_SLEEP:
            digitalWrite(_m0, HIGH);
            digitalWrite(_m1, HIGH);
            break;
        case MODE_NORMAL:
            digitalWrite(_m0, LOW);
            digitalWrite(_m1, LOW);
            break;
        case MODE_CONFIG:
            digitalWrite(_m0, HIGH);
            digitalWrite(_m1, LOW);
            break;
        case MODE_WAKEUP:
            digitalWrite(_m0, LOW);
            digitalWrite(_m1, HIGH);
            break;
    }
    waitAux();
}
void Lora::waitAux() {
    while (!digitalRead(_aux)) 
        vTaskDelay(pdMS_TO_TICKS(1));
}
void Lora::send(const DataPacket& data) {
    waitAux();
    _serial.write((uint8_t*)&data, sizeof(DataPacket));
    waitAux();
}
bool Lora::receive(DataPacket& out) {
    if (_serial.available() < sizeof(DataPacket))
        return false;

    _serial.readBytes((uint8_t*)&out, sizeof(DataPacket));
    return true;
}