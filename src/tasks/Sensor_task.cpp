#include "Sensor_task.hpp"
#include "Lora.hpp"
#include "RTC.hpp"
#include "Temperature.hpp"
#include "Turbidity.hpp"
#include <RTClib.h>

extern Turbidity turbidity;
extern Temperature temperature;

void SensorTask(void* pvParameters) {

    SensorTaskContext* sensor = static_cast<SensorTaskContext*>(pvParameters);
    QueueHandle_t txQueue = sensor->txQueue;

    uint16_t packetId = 0;

    for(;;){
        
        RTC_Time current;
        getTimestamp(&current);

        float turbidityValue = turbidity.get();
        float temperaturevalue = temperature.getTemperatureC();

        DataPacket packet;
        packet.id = (uint16_t)packetId++;
        packet.temperature = (float_t)temperaturevalue;
        packet.turbidity = (float_t)turbidityValue;
        packet.timestamp = (RTC_Time)current; 
        
        xQueueSend(txQueue, &packet, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}