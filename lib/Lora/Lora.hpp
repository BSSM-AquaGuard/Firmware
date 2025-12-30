#pragma once
#include <cstdint>
#include <HardwareSerial.h>
#include "RTC.hpp"

#pragma pack(push, 1)
struct DataPacket {
    uint16_t id;
    float ph;
    float temperature;
    float turbidity;
    uint32_t timestamp; 
};
#pragma pack(pop)
static_assert(sizeof(DataPacket) == 18, "DataPacket must be 14 bytes");

enum Mode {
    MODE_SLEEP,
    MODE_NORMAL,
    MODE_CONFIG,
    MODE_WAKEUP
};

class Lora {
    public:
        Lora(HardwareSerial& serial, uint8_t m0, uint8_t m1, uint8_t aux);
        void begin();
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