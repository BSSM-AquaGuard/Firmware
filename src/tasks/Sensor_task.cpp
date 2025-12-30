#include "Sensor_task.hpp"
#include "Lora.hpp"
#include "RTC.hpp"
#include "Temperature.hpp"
#include "Turbidity.hpp"
#include <RTClib.h>
#include <PH.hpp>

#include "pin.h"

void SensorTask(void* pvParameters) {

    SensorTaskContext* sensor = static_cast<SensorTaskContext*>(pvParameters);
    QueueHandle_t txQueue = sensor->txQueue;
    Turbidity turbidity(TURBIDITY_SENSOR_PIN);
    OneWire oneWire(TEMPERATURE_SENSOR_PIN);
    Temperature temperature(oneWire);
    PH potentialofhydrogen(PH_SIG);
    
    // 생수 보정: 0.5V에서 pH 7.0 기준
    potentialofhydrogen.calibrate(-3.5, 8.75);

    uint16_t packetId = 0;

    for(;;){
        
        float turbidityValue = turbidity.get();
        float temperaturevalue = temperature.getTemperatureC();
        float potentialofhydrogenvalue = potentialofhydrogen.get();
        
        // 범위 제한 (0-14)
        if (potentialofhydrogenvalue < 0) potentialofhydrogenvalue = 0;
        if (potentialofhydrogenvalue > 14) potentialofhydrogenvalue = 14;

        DataPacket packet;
        packet.id = packetId++;
        packet.ph = potentialofhydrogenvalue;
        packet.temperature = temperaturevalue;
        packet.turbidity = turbidityValue;
        packet.timestamp = getTimestampToUnix(); 

        Serial.print("[Sensor] ID: ");
        Serial.print(packet.id);
        Serial.print(", pH: ");
        Serial.print(packet.ph);
        Serial.print(" (raw V: ");
        Serial.print((packet.ph - 8.75) / -3.5, 3);
        Serial.print("), Temp: ");
        Serial.print(packet.temperature);
        Serial.print(" C, Turbidity: ");
        Serial.print(packet.turbidity);
        Serial.print(" V, Timestamp: ");
        Serial.println(packet.timestamp);
        
        xQueueSend(txQueue, &packet, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}