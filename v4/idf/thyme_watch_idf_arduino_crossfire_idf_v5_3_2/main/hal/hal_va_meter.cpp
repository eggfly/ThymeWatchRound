
#include <Arduino.h>
#include "common.h"
#include "hal_va_meter.h"
#include <Adafruit_INA219.h>

#define TAG "VA_METER"

Adafruit_INA219 ina219(INA219_ADDRESS);

void init_va_meter()
{
  if (!ina219.begin(&Wire1))
  {
    MY_LOG("Failed to find INA219 chip");
    // Serial.println("Failed to find INA219 chip");
    while (1)
    {
      delay(10);
    }
  }
  // ina219.setCalibration_32V_2A();
}

va_meter_t read_va_meter()
{
  va_meter_t va_meter;
  va_meter.shuntvoltage = ina219.getShuntVoltage_mV();
  va_meter.busvoltage = ina219.getBusVoltage_V();
  va_meter.current_mA = ina219.getCurrent_mA();
  va_meter.power_mW = ina219.getPower_mW();
  va_meter.loadvoltage = va_meter.busvoltage + (va_meter.shuntvoltage / 1000);
  return va_meter;
}

void print_va_meter(va_meter_t va_meter)
{
  MY_LOG("Bus Voltage: %.3f V, Shunt Voltage: %.3f mV, Load Voltage: %.3f V, Current: %.3f mA, Power: %.3f mW", va_meter.busvoltage, va_meter.shuntvoltage, va_meter.loadvoltage, va_meter.current_mA, va_meter.power_mW);
}
