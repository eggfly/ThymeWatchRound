#pragma once

#include "Arduino.h"
#include <framework/ThymeApp.h>
#include "Arduino_Canvas_6bit.h"
#include <TimeLib.h> //https://github.com/PaulStoffregen/Time

#include "hal/hal_digital_crown.h"
#include "hal/hal_battery_fuel_gauge.h"
#include "hal/hal_pressure_sensor.h"
#include "hal/hal_va_meter.h"
#include "hal/hal_imu.h"

class WatchFaceSelector : public ThymeApp
{
public:
    WatchFaceSelector(void *params);
    std::string appId() override;
    void onStart(Arduino_Canvas_6bit *gfx) override;
    void onStop(Arduino_Canvas_6bit *gfx) override;
    void onDraw(Arduino_Canvas_6bit *gfx) override;
    ~WatchFaceSelector() override;

private:
    uint16_t nyanCatFramePos = 0;
    void renderNyanCat(Arduino_Canvas_6bit *gfx);
};
