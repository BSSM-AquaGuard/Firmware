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

        Serial.print("[Sensor] ID: ");
        Serial.print(packet.id);
        Serial.print(", Temp: ");
        Serial.print(packet.temperature);
        Serial.print(" C, Turbidity: ");
        Serial.print(packet.turbidity);
        Serial.print(" V, Timestamp: ");
        Serial.println(packet.timestamp);
        
        xQueueSend(txQueue, &packet, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}