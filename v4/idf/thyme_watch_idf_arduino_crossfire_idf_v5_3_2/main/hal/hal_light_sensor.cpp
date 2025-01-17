
#include <Arduino.h>
#include <opt3001.h>
#include <Wire.h>

opt3001 opt3001_sensor;

void init_opt3001()
{
  opt3001_sensor.setup(Wire, 0x44);
  auto success = opt3001_sensor.detect() == 0;
  Serial.printf("OPT3001 detected: %s\n", success ? "true" : "false");
  opt3001_sensor.config_set(OPT3001_CONVERSION_TIME_100MS);
}

void sampling_opt3001()
{
  float lux;
  opt3001_sensor.conversion_continuous_enable();
  // delay(200);
  auto ok = opt3001_sensor.lux_read(&lux) == 0;
  Serial.printf("Lux: %.2f, success=%s\n", lux, ok ? "true" : "false");
}
