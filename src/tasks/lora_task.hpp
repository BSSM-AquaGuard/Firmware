#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <Lora.hpp>

struct LoRaTaskContext {
    Lora* lora;
    QueueHandle_t txQueue;
};

void LoRaTxTask(void* pvParameters);