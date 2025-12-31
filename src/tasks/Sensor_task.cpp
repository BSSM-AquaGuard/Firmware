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

    // 1회만 측정 및 전송 후 deep sleep 진입
    float turbidityValue = turbidity.get();
    float temperaturevalue = temperature.getTemperatureC();
    float potentialofhydrogenvalue = potentialofhydrogen.get();
    uint8_t redTideStatus = digitalRead(RED_TIDE_PIN) ? 1 : 0;  // 단순 디지털 입력

    // 범위 제한 (0-14)
    if (potentialofhydrogenvalue < 0) potentialofhydrogenvalue = 0;
    if (potentialofhydrogenvalue > 14) potentialofhydrogenvalue = 14;

    DataPacket packet;
    packet.id = packetId++;
    packet.ph = potentialofhydrogenvalue;
    packet.temperature = temperaturevalue;
    packet.turbidity = turbidityValue;
    packet.timestamp = getTimestampToUnix();
    packet.redTide = redTideStatus; 

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
    Serial.print(" V, RedTide: ");
    Serial.print(packet.redTide ? "DETECTED" : "Normal");
    Serial.print(", Timestamp: ");
    Serial.println(packet.timestamp);

    xQueueSend(txQueue, &packet, portMAX_DELAY);

    Serial.println("[Sensor] Going to deep sleep for 10 seconds...");
    vTaskDelay(pdMS_TO_TICKS(1000)); // 전송 후 약간의 여유
    esp_sleep_enable_timer_wakeup(10ULL * 1000000ULL); // 10초 후 wakeup
    esp_deep_sleep_start();
}