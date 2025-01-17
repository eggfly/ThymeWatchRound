
#include <Arduino.h>

#define SENSOR_POWER_PIN (9)

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  Serial.println("Starting Serial");
}

void loop() {
  delay(10);
}
