#pragma once
#include <cstdint>
#include <HardwareSerial.h>
#include "RTC.hpp"

struct DataPacket {
    uint16_t id;
    float_t temperature;
    float_t turbidity;
    RTC_Time timestamp; 
};

enum Mode {
    MODE_SLEEP,
    MODE_NORMAL,
    MODE_CONFIG,
    MODE_WAKEUP
};

class Lora {
    public:
        Lora(HardwareSerial& serial, uint8_t m0, uint8_t m1, uint8_t aux);
        void send(const DataPacket& data);
        bool receive(DataPacket& out);
        void setMode(Mode mode);
    private:
        HardwareSerial& _serial;
        uint8_t _m0;
        uint8_t _m1;
        uint8_t _aux;
        void waitAux();
};