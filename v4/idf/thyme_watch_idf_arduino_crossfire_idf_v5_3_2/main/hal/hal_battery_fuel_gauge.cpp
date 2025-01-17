
#include <Arduino.h>
#include "hal_battery_fuel_gauge.h"

#define TAG "FUEL_GAUGE"

Adafruit_MAX17048 maxlipo;

void init_fuel_gauge()
{
  while (!maxlipo.begin())
  {
    MY_LOG("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!");
    delay(2000);
  }
  MY_LOG("Found MAX17048 with Chip ID: 0x%02X", maxlipo.getChipID());
}

bool read_fuel_gauge(fuel_gauge_t *fuel_gauge)
{
  float cellVoltage = maxlipo.cellVoltage();
  if (isnan(cellVoltage))
  {
    MY_LOG("Failed to read cell voltage, check battery is connected!");
    return false;
  }
  fuel_gauge->cellVoltage = cellVoltage;
  fuel_gauge->cellPercent = maxlipo.cellPercent();
  fuel_gauge->chargeRate = maxlipo.chargeRate();
  return true;
}

void print_fuel_gauge(fuel_gauge_t fuel_gauge)
{
  MY_LOG("Batt Voltage: %f V", fuel_gauge.cellVoltage);
  MY_LOG("Batt Percent: %f %%", fuel_gauge.cellPercent);
  MY_LOG("Charge Rate: %f %%/h", fuel_gauge.chargeRate);
}

void hibernate_fuel_gauge()
{
  maxlipo.hibernate();
}