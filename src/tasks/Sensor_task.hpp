#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "RTC.hpp"
#include "PH.hpp"

struct SensorTaskContext{
    QueueHandle_t txQueue;
};

void SensorTask(void* pvParameters);