#include <Arduino.h>
#include <OneWire.h>
#include <DS18B20.h>

OneWire oneWire(4);
DS18B20 DS18B20_Sensor(&oneWire);

void setup() {
  Serial.begin(115200);
  DS18B20_Sensor.begin();
}

void loop() {
  DS18B20_Sensor.requestTemperatures();
  float temp = DS18B20_Sensor.getTempC();
  Serial.println(temp);
  delay(1000);
}