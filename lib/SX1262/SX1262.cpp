#include "SX1262.hpp"
#include <freertos/FreeRTOS.h>

SX1262::SX1262(HardwareSerial& serial, uint8_t m0, uint8_t m1, uint8_t aux)
    : Serial(serial), _m0(m0), _m1(m1), _aux(aux) {
        Serial.begin(9600);
        pinMode(_m0, OUTPUT);
        pinMode(_m1, OUTPUT);
        pinMode(_aux, INPUT_PULLUP);
}
void SX1262::setMode(Mode mode) {
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
void SX1262::waitAux() {
    while (!digitalRead(_aux)) 
        vTaskDelay(pdMS_TO_TICKS(1));
}
void SX1262::sendData(const DataPacket& data) {
    waitAux();
    Serial.write((uint8_t*)&data, sizeof(DataPacket));
    waitAux();
}
