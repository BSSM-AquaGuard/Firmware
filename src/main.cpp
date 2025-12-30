#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <Lora.hpp>
#include "pin.h"
#include "RTC.hpp"
#include "PH.hpp"

#include "tasks/lora_task.hpp"
#include "tasks/Sensor_task.hpp"  

PH Ph(34);
QueueHandle_t txQueue;
LoRaTaskContext loraCtx;
SensorTaskContext sensorCtx;

Lora lora(Serial2, LORA_M0_PIN, LORA_M1_PIN, LORA_AUX_PIN);

/* void scanI2C() {
  Serial.println("\n[I2C Scanner] Scanning...");
  
  int nDevices = 0;
  for(byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("[I2C Scanner] Device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    } else if (error == 4) {
      Serial.printf("[I2C Scanner] Unknown error at 0x%02X\n", address);
    }
  }
  
  if (nDevices == 0) {
    Serial.println("[I2C Scanner] No I2C devices found!");
    Serial.println("[I2C Scanner] Check: VCC, GND, SDA, SCL connections");
    Serial.println("[I2C Scanner] Check: Pull-up resistors on SDA/SCL");
  } else {
    Serial.printf("[I2C Scanner] Found %d device(s)\n", nDevices);
  }
}*/

void setup() {
  Serial.begin(115200);
  /*delay(1000);
  
  // Lolin D32: I2C 클럭 속도를 낮춰서 시도
  Wire.begin();
  Wire.setClock(10000);  // 10kHz로 낮춤 (기본 100kHz)
  Serial.printf("[I2C] Using default board pins - SDA=%d, SCL=%d at 10kHz\n", SDA, SCL);
  
  // scanI2C();  // I2C 스캔 먼저 실행
  
  Serial2.begin(9600, SERIAL_8N1, LORA_SERIAL_RX_PIN, LORA_SERIAL_TX_PIN);
  
  rtc_init();  // RTC 초기화 추가
  lora.begin();
  txQueue = xQueueCreate(10, sizeof(DataPacket));
  
  loraCtx.lora = &lora;
  loraCtx.txQueue = txQueue;

  xTaskCreatePinnedToCore(
    LoRaTxTask,
    "LoRaTxTask",
    4096,
    &loraCtx,
    1,
    NULL,
    1
  );

  sensorCtx.txQueue = txQueue;
  xTaskCreatePinnedToCore(
    SensorTask,
    "SensorTask",
    8192,
    &sensorCtx,
    1,
    NULL,
    0
  );*/
}

void loop() {
  Serial.println(Ph.get());
}
