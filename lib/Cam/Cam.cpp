#include "Cam.hpp"

Cam::Cam(HardwareSerial& serial) : _serial(serial) {
    _lastData.detected = 0;
}

void Cam::begin() {
    // UART는 main에서 이미 초기화됨
}

bool Cam::getData(CamData& out) {
    // 지원하는 입력 형식:
    // 1) 프레임: [0xA5][status] (status: 0/1)
    // 2) 단일 문자: 'R'/'r' => 1, 그 외('G','B','N','0') => 0

    while (_serial.available()) {
        uint8_t b = _serial.read();

        // 디버그: 들어오는 모든 바이트 로깅
        Serial.printf("[CAM RAW] 0x%02X\n", b);

        // 개행/공백류는 무시
        if (b == '\r' || b == '\n' || b == ' ') {
            continue;
        }

        // 케이스 1: 헤더 기반 프레임
        if (b == CAM_FRAME_HEADER) {
            if (!_serial.available()) return false; // 상태 바이트 대기
            uint8_t status = _serial.read();
            out.detected = status ? 1 : 0;
            _lastData = out;
            Serial.print("[CAM] Frame: header 0xA5, status=");
            Serial.println(out.detected ? "RED TIDE DETECTED" : "Normal");
            return true;
        }

        // 케이스 2: 문자 직접 전송 (라인 내 숫자/문자열 섞여도 첫 R/N만 본다)
        if (b == 'R' || b == 'r') {
            out.detected = 1;
            _lastData = out;
            Serial.println("[CAM] Char: R/r -> RED TIDE DETECTED");
            // 해당 라인의 나머지는 의미 없으니 비움
            while (_serial.available()) {
                uint8_t c = _serial.peek();
                if (c == '\n' || c == '\r') { _serial.read(); break; }
                _serial.read();
            }
            return true;
        }
        if (b == 'G' || b == 'g' || b == 'B' || b == 'b' || b == 'N' || b == 'n' || b == '0') {
            out.detected = 0;
            _lastData = out;
            Serial.println("[CAM] Char: non-R -> Normal");
            while (_serial.available()) {
                uint8_t c = _serial.peek();
                if (c == '\n' || c == '\r') { _serial.read(); break; }
                _serial.read();
            }
            return true;
        }

        // 그 외 바이트는 무시 (노이즈/로그)
    }
    return false;
}

uint8_t Cam::getStatus() {
    CamData data;
    if (getData(data)) {
        return data.detected;
    }
    // 새 데이터 없으면 마지막 상태 반환
    return _lastData.detected;
}
