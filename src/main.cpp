#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <Lora.hpp>
#include "pin.h"

Lora lora(Serial2, LORA_M0_PIN, LORA_M1_PIN, LORA_AUX_PIN);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, LORA_SERIAL_RX_PIN, LORA_SERIAL_TX_PIN); 
  lora.setMode(MODE_NORMAL);
}

void loop() {}
