#pragma once

#include <Adafruit_INA219.h>

typedef struct
{
    float shuntvoltage;
    float busvoltage;
    float current_mA;
    float loadvoltage;
    float power_mW;
} va_meter_t;

void init_va_meter();
va_meter_t read_va_meter();
void print_va_meter(va_meter_t va_meter);
