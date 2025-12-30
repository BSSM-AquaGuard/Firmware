#include "lora_task.hpp"

void LoRaTxTask(void* pvParameters) {
    DataPacket packet;
    LoRaTaskContext* ctx = static_cast<LoRaTaskContext*>(pvParameters);
    Lora* lora = ctx->lora;
    QueueHandle_t txQueue = ctx->txQueue;

    lora->setMode(MODE_NORMAL);

    for(;;) {
        if(xQueueReceive(txQueue, &packet, portMAX_DELAY) == pdTRUE) {
            lora->send(packet);
             Serial.print("[LoRa TX] ");
            Serial.println(packet.id);
        }
    }
}