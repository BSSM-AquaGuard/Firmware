#pragma once
#include <cstdint>
#include <HardwareSerial.h>

struct DataPacket {
    uint16_t id;
    uint16_t counter;
};

enum Mode {
    MODE_SLEEP,
    MODE_NORMAL,
    MODE_CONFIG,
    MODE_WAKEUP
};

class SX1262 {
    public:
        SX1262(HardwareSerial& serial, uint8_t m0, uint8_t m1, uint8_t aux);
        void sendData(const DataPacket& data);
        bool receiveData(DataPacket& out);
        void setMode(Mode mode);
    private:
        HardwareSerial& Serial;
        uint8_t _m0;
        uint8_t _m1;
        uint8_t _aux;
        void waitAux();
};