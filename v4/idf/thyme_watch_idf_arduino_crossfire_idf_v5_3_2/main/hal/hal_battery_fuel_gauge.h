#pragma once

#include <Arduino.h>
#include "common.h"
#include "Adafruit_MAX1704X.h"

typedef struct
{
    float cellVoltage;
    float cellPercent;
    float chargeRate;
} fuel_gauge_t;

void init_fuel_gauge();
bool read_fuel_gauge(fuel_gauge_t *data);
void print_fuel_gauge(fuel_gauge_t data);
void hibernate_fuel_gauge();