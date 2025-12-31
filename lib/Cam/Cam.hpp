#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

// ESP32-CAM에서 보내는 적조 판단 데이터
struct CamData {
    uint8_t detected;  // 0: 정상, 1: 적조 감지
};

constexpr uint8_t CAM_FRAME_HEADER = 0xA5;  // 프레임 헤더 바이트

class Cam {
public:
    Cam(HardwareSerial& serial);
    void begin();
    bool getData(CamData& out);  // 데이터 수신 성공 시 true
    uint8_t getStatus();  // 마지막 상태 반환 (0 or 1)
    
private:
    HardwareSerial& _serial;
    CamData _lastData;
};
