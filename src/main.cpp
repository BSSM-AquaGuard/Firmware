#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <Lora.hpp>
#include "pin.h"
#include "RTC.hpp"

#include "tasks/lora_task.hpp"
#include "tasks/Sensor_task.hpp"

QueueHandle_t txQueue;
LoRaTaskContext loraCtx;
SensorTaskContext sensorCtx;

Lora lora(Serial2, LORA_M0_PIN, LORA_M1_PIN, LORA_AUX_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Lolin D32: I2C 클럭 속도를 낮춰서 시도
  Wire.begin();
  Wire.setClock(10000);  // 10kHz로 낮춤 (기본 100kHz)
  
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
  );
}

void loop() {}
