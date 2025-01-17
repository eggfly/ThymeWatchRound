
#include "hal_pressure_sensor.h"
#include "common.h"

#define TAG "Pressure"

Adafruit_BMP280 bmp(&Wire1); // Secondary I2C

void init_pressure_sensor()
{
  auto status = bmp.begin(BMP280_ADDRESS_ALT);
  if (!status)
  {
    MY_LOG("Could not find a valid BMP280 sensor, check wiring or try a different address!");
    MY_LOG("SensorID was: 0x%02X", bmp.sensorID());
    MY_LOG("ID of 0xFF probably means a bad address, a BMP 180 or BMP 085");
    MY_LOG("ID of 0x56-0x58 represents a BMP280,");
    MY_LOG("ID of 0x60 represents a BME280.");
    MY_LOG("ID of 0x61 represents a BME680.");
    while (1)
    {
      delay(1000);
      MY_LOG("BMP280 sensor not found.");
    }
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

pressure_t read_pressure()
{
  pressure_t result;
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float altitude = bmp.readAltitude(1013.25); /* Adjusted to local forecast! */
  MY_LOG("Temperature = %f *C", temperature);
  MY_LOG("Pressure = %f Pa", pressure);
  MY_LOG("Approx altitude = %f m", altitude);
  result.altitude = altitude;
  result.pressure = pressure;
  result.temperature = temperature;
  return result;
}
