#include <Arduino.h>
#include <Temperature.hpp>

Temperature Sensor(4);

void setup() {
  Serial.begin(115200);
}

void loop() {
  float temp = Sensor.getTemperatureC();
  Serial.println(temp);
  delay(1000);
}