#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "RTC.hpp"
#include "RTC.hpp"

struct SensorTaskContext{
    QueueHandle_t txQueue;
};

void SensorTask(void* pvParameters);