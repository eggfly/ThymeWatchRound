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

class ThymeWatchFace : public ThymeApp
{
public:
    ThymeWatchFace();
    std::string appId() override;
    void onStart(Arduino_Canvas_6bit *gfx) override;
    void onStop(Arduino_Canvas_6bit *gfx) override;
    void onDraw(Arduino_Canvas_6bit *gfx) override;
    void onBackPressed() override;
    void onUpButtonPressed() override;
    void onMiddleButtonPressed() override;
    void onDownButtonPressed() override;
    bool needUpdateDigitalCrown() override { return true; } 
    void onDigitalCrownRotated(long position) override;
    ~ThymeWatchFace() override;

private:
    const int16_t CrosshairHalfLength = 20;
    const int16_t CrosshairHalfThickness = 1;
    const int16_t CrosshairCenterRadius = 4;
    long lastDigitalCrownPosition = 0;
    int16_t currentWatchFaceIndex = 0;
    int16_t watchFaceCount = 3;
    va_meter_t va_meter_data;
    fuel_gauge_t fuel_gauge_data;
    imu_t imu_data;
    uint16_t nyanCatFramePos = 0;
    tmElements_t tm;
    unsigned long rtcUpdateTime = 0;
    unsigned long sensorUpdateTime = 0;
    unsigned long imuUpdateTime = 0;
    void renderNyanCat(Arduino_Canvas_6bit *gfx);
    void drawIMUPointer(Arduino_Canvas_6bit *gfx);
    char buf[256];
    char power_buf[256];
};
