#pragma once
#include <Arduino.h>
#include <Adafruit_BMP280.h>

typedef struct
{
    float temperature;
    float pressure;
    float altitude;
} pressure_t;

void init_pressure_sensor();
pressure_t read_pressure();
