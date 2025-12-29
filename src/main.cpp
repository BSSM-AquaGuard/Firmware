#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#include <SX1262.hpp>

SX1262 sx1262(Serial2, 15, 2, 26);

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, 16, 17); 
    sx1262.setMode(MODE_NORMAL);
}

void loop() {
    DataPacket packet;
    if(sx1262.receiveData(packet)) {
        Serial.print("Received packet: ");
        Serial.println(packet.counter);
    }
    delay(1000);
}
