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
    uint8_t redTide;  // 0: 정상, 1: 적조 감지
};
#pragma pack(pop)
static_assert(sizeof(DataPacket) == 19, "DataPacket must be 19 bytes");

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