#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <SX1262.hpp>

SX1262 sx1262(Serial2, 15, 2, 26);

uint32_t cnt = 0;

void setup() {
    Serial.begin(115200);

    // ESP32 Serial2 핀 지정 (중요)
    Serial2.begin(9600, SERIAL_8N1, 16, 17); 
    // RX=16, TX=17 (네 보드에 맞게 바꿔도 됨)

    sx1262.setMode(MODE_NORMAL);

    Serial.println("SX1262 TEST START");
}

void loop() {
    DataPacket packet;
    packet.id = 0x01;
    packet.counter = cnt++;

    sx1262.sendData(packet);

    Serial.print("Sent packet: ");
    Serial.println(packet.counter);

    delay(1000);
}
