#include "Sensor_task.hpp"
#include "Lora.hpp"
#include "RTC.hpp"
#include "Temperature.hpp"
#include "Turbidity.hpp"
#include <RTClib.h>

#include "pin.h"

void SensorTask(void* pvParameters) {

    SensorTaskContext* sensor = static_cast<SensorTaskContext*>(pvParameters);
    QueueHandle_t txQueue = sensor->txQueue;
    Turbidity turbidity(TURBIDITY_SENSOR_PIN);
    OneWire oneWire(TEMPERATURE_SENSOR_PIN);
    Temperature temperature(oneWire);

    uint16_t packetId = 0;

    for(;;){
        

        float turbidityValue = turbidity.get();
        float temperaturevalue = temperature.getTemperatureC();

        DataPacket packet;
        packet.id = packetId++;
        packet.temperature = temperaturevalue;
        packet.turbidity = turbidityValue;
        packet.timestamp = getTimestampToUnix(); 
        
        xQueueSend(txQueue, &packet, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}