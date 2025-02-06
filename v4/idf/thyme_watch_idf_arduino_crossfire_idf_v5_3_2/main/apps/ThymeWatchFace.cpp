#include "ThymeWatchFace.h"
#include <common.h>
#include <config.h>
#include "hal/hal_ls012b7dd06.h"
#include "hal/hal_rtc.h"
#include <RasterGothic18CondBold18pt7b.h>

#include "hal/hal_ls012b7dd06.h"
#include "resources/casio_yellow.h"
#include "resources/lcd_digits.h"
#include "resources/anim_data_12_frames.h"

#include "util/ui.h"
#include "util/system.h"

#include <apps/apps.h>
#include "framework/app_manager.h"

#define TAG "ThymeWatchFace"

using namespace Thyme;

ThymeWatchFace::ThymeWatchFace()
{
    MY_LOG("ThymeWatchFace::ThymeWatchFace()");
}

std::string ThymeWatchFace::appId()
{
    return "watch_face";
}

void ThymeWatchFace::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("ThymeWatchFace::onStart()");
}

void ThymeWatchFace::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("ThymeWatchFace::onStop()");
}

void ThymeWatchFace::onDraw(Arduino_Canvas_6bit *gfx)
{
    unsigned long currTime = millis();
    if (currTime - rtcUpdateTime > RTC_UPDATE_INTERVAL)
    {
        tm = readRTC();
        rtcUpdateTime = currTime;
    }

    if (currTime - sensorUpdateTime > SENSOR_UPDATE_INTERVAL)
    {
        pressure_t pressure_data = read_pressure();
        snprintf(buf, sizeof(buf), "\n T: %.3f C\n P: %.3f Pa\n A: %.3f m",
                 pressure_data.temperature,
                 pressure_data.pressure,
                 pressure_data.altitude);
        // tm = readRTC();
        // printDateTime(tm);
        va_meter_data = read_va_meter();
        print_va_meter(va_meter_data);
        // snprintf(va_buf, sizeof(va_buf), "Bus Voltage: %.3f V, Shunt Voltage: %.2f mV, Load Voltage: %.2f V, Current: %.2f mA, Power: %.2f mW", va_meter_data.busvoltage, va_meter_data.shuntvoltage, va_meter_data.loadvoltage, va_meter_data.current_mA, va_meter_data.power_mW);
        // snprintf(va_buf, sizeof(va_buf), "%.3fV/%.2fmA", va_meter_data.loadvoltage, va_meter_data.current_mA);
        // MY_LOG("%s", va_buf);
        read_fuel_gauge(&fuel_gauge_data);
        print_fuel_gauge(fuel_gauge_data);
        snprintf(power_buf, sizeof(power_buf), "%.2fV %.2fmA %.1f%%", fuel_gauge_data.cellVoltage, va_meter_data.current_mA, fuel_gauge_data.cellPercent);
        sensorUpdateTime = currTime;
    }

    if (currTime - imuUpdateTime > IMU_UPDATE_INTERVAL)
    {
        read_imu(&imu_data);
        if (imu_data.acc_valid && imu_data.gyro_valid)
        {
            MY_LOG("IMU: %f %f %f, %f %f %f", imu_data.acc_x, imu_data.acc_y, imu_data.acc_z, imu_data.gyro_x, imu_data.gyro_y, imu_data.gyro_z);
        }
        else
        {
            MY_LOG("IMU data not available");
        }
        imuUpdateTime = currTime;
    }

    if (currentWatchFaceIndex == 0)
    {
        // Start: renderCasioWatchFace
        memcpy(gfx->getFramebuffer(), casio_yellow, 240 * 240);
        // 小时的第1位
        gfx->drawBitmap(46, 114, digits_20x44[tm.Hour / 10], 20, 44, RGB565_BLACK);
        // 小时的第2位
        gfx->drawBitmap(71, 114, digits_20x44[tm.Hour % 10], 20, 44, RGB565_BLACK);
        // 分钟的第1位
        gfx->drawBitmap(107, 114, digits_20x44[tm.Minute / 10], 20, 44, RGB565_BLACK);
        // 分钟的第2位
        gfx->drawBitmap(132, 114, digits_20x44[tm.Minute % 10], 20, 44, RGB565_BLACK);
        // 秒钟的第1位
        gfx->drawBitmap(160, 129, digits_14x29[tm.Second / 10], 14, 29, RGB565_BLACK);
        // 秒钟的第2位
        gfx->drawBitmap(178, 129, digits_14x29[tm.Second % 10], 14, 29, RGB565_BLACK);
        printCenteredText(gfx, power_buf, COLOR_BLACK, 1, 120, 120);
        drawIMUPointer(gfx);
        flushDisplay(gfx->getFramebuffer());
    }
    else if (currentWatchFaceIndex == 1)
    {
        renderNyanCat(gfx);
        drawIMUPointer(gfx);
    }
    else if (currentWatchFaceIndex == 2)
    {
        time_t unix = makeTime(tm);
        unix += lastDigitalCrownPosition * 60;
        tmElements_t adjustedTime;
        breakTime(unix, adjustedTime);

        gfx->fillScreen(RGB565_BLACK);
        // Draw 12 hour digits
        std::string hours[12] = {"12", "1", "2", "3", "4", "5",
                                 "6", "7", "8", "9", "10", "11"};
        for (int i = 0; i < 12; i++)
        {
            int16_t x = 120 + 100 * cos(i * 30 * DEG_TO_RAD - HALF_PI);
            int16_t y = 120 + 100 * sin(i * 30 * DEG_TO_RAD - HALF_PI);
            printCenteredText(gfx, hours[i].c_str(), RGB565_WHITE, 2, x, y);
        }
        // hours, minutes, seconds pointer watch face
        // Hour
        int16_t hourThinkness = 4;
        int16_t hourRadius = 50;
        int16_t hourAngle = (adjustedTime.Hour % 12) * 30 + (adjustedTime.Minute / 2);
        int16_t hour_x0 = 120 - hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y0 = 120 - hourThinkness * sin(hourAngle * DEG_TO_RAD);
        int16_t hour_x1 = 120 + hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y1 = 120 + hourThinkness * sin(hourAngle * DEG_TO_RAD);
        int16_t hour_x = 120 + hourRadius * cos(hourAngle * DEG_TO_RAD - HALF_PI);
        int16_t hour_y = 120 + hourRadius * sin(hourAngle * DEG_TO_RAD - HALF_PI);
        int16_t hour_x2 = hour_x + hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y2 = hour_y + hourThinkness * sin(hourAngle * DEG_TO_RAD);
        int16_t hour_x3 = hour_x - hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y3 = hour_y - hourThinkness * sin(hourAngle * DEG_TO_RAD);
        gfx->fillTriangle(hour_x0, hour_y0, hour_x1, hour_y1, hour_x2, hour_y2, RGB565_WHITE);
        gfx->fillTriangle(hour_x2, hour_y2, hour_x3, hour_y3, hour_x0, hour_y0, RGB565_WHITE);

        // Minute
        int16_t minuteThicknes = 3;
        int16_t minuteRadius = 70;
        int16_t minuteAngle = adjustedTime.Minute * 6;
        int16_t minute_x0 = 120 - minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y0 = 120 - minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        int16_t minute_x1 = 120 + minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y1 = 120 + minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        int16_t minute_x = 120 + minuteRadius * cos(minuteAngle * DEG_TO_RAD - HALF_PI);
        int16_t minute_y = 120 + minuteRadius * sin(minuteAngle * DEG_TO_RAD - HALF_PI);
        int16_t minute_x2 = minute_x + minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y2 = minute_y + minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        int16_t minute_x3 = minute_x - minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y3 = minute_y - minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        gfx->fillTriangle(minute_x0, minute_y0, minute_x1, minute_y1, minute_x2, minute_y2, RGB565_GREEN);
        gfx->fillTriangle(minute_x2, minute_y2, minute_x3, minute_y3, minute_x0, minute_y0, RGB565_GREEN);

        // Seconds
        int16_t secondRadius = 90;
        int16_t secondAngle = adjustedTime.Second * 6;
        bool thinest = true;
        if (thinest)
        {
            int16_t second_x = 120 + secondRadius * cos(secondAngle * DEG_TO_RAD - HALF_PI);
            int16_t second_y = 120 + secondRadius * sin(secondAngle * DEG_TO_RAD - HALF_PI);
            gfx->drawLine(120, 120, second_x, second_y, RGB565_RED);
        }
        else
        {
            int16_t secondThicknes = 2;
            int16_t second_x0 = 120 - secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y0 = 120 - secondThicknes * sin(secondAngle * DEG_TO_RAD);
            int16_t second_x1 = 120 + secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y1 = 120 + secondThicknes * sin(secondAngle * DEG_TO_RAD);
            int16_t second_x = 120 + secondRadius * cos(secondAngle * DEG_TO_RAD - HALF_PI);
            int16_t second_y = 120 + secondRadius * sin(secondAngle * DEG_TO_RAD - HALF_PI);
            int16_t second_x2 = second_x + secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y2 = second_y + secondThicknes * sin(secondAngle * DEG_TO_RAD);
            int16_t second_x3 = second_x - secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y3 = second_y - secondThicknes * sin(secondAngle * DEG_TO_RAD);
            gfx->fillTriangle(second_x0, second_y0, second_x1, second_y1, second_x2, second_y2, RGB565_RED);
            gfx->fillTriangle(second_x2, second_y2, second_x3, second_y3, second_x0, second_y0, RGB565_RED);
        }
        gfx->fillCircle(120, 120, hourThinkness, RGB565_RED);
        drawIMUPointer(gfx);
        flushDisplay(gfx->getFramebuffer());
    }
    else
    {
        MY_LOG("Unknown watch face index: %d", currentWatchFaceIndex);
    }
}

void ThymeWatchFace::drawIMUPointer(Arduino_Canvas_6bit *gfx)
{
    // x 取负的
    int16_t x = 120 - 120 * imu_data.acc_x;
    int16_t y = 120 + 120 * imu_data.acc_y;
    // horizontal crosshair
    gfx->fillRect(x - CrosshairHalfLength, y - CrosshairHalfThickness, 2 * CrosshairHalfLength, 2 * CrosshairHalfThickness, RGB565_GREEN);
    // vertical crosshair
    gfx->fillRect(x - CrosshairHalfThickness, y - CrosshairHalfLength, 2 * CrosshairHalfThickness, 2 * CrosshairHalfLength, RGB565_GREEN);
    gfx->fillCircle(x, y, CrosshairCenterRadius, RGB565_RED);
}

void ThymeWatchFace::onUpButtonPressed()
{
    currentWatchFaceIndex++;
    // watchFaceCount
    if (currentWatchFaceIndex >= 3)
    {
        currentWatchFaceIndex = 0;
    }
    MY_LOG("ThymeWatchFace::onUpButtonPressed()");
}

void ThymeWatchFace::onDigitalCrownRotated(long position)
{
    lastDigitalCrownPosition = position;
}

void ThymeWatchFace::onMiddleButtonPressed()
{
    AppManager::navigateToApp<ThymeWatchMenu>();
    MY_LOG("ThymeWatchFace::onMiddleButtonPressed()");
}

void ThymeWatchFace::onDownButtonPressed()
{
    currentWatchFaceIndex--;
    if (currentWatchFaceIndex < 0)
    {
        currentWatchFaceIndex = watchFaceCount - 1;
    }
    MY_LOG("ThymeWatchFace::onDownButtonPressed()");
}

ThymeWatchFace::~ThymeWatchFace()
{
    MY_LOG("ThymeWatchFace::~ThymeWatchFace()");
}

void ThymeWatchFace::onBackPressed()
{
    toggleBacklight();
    MY_LOG("ThymeWatchFace::onBackPressed()");
}

void ThymeWatchFace::renderNyanCat(Arduino_Canvas_6bit *gfx)
{
    uint16_t nyanCatFrameCount = sizeof(anim_data) / sizeof(anim_data[0]);
    MY_LOG("nyanCatFrameCount: %d", nyanCatFrameCount);
    memcpy(gfx->getFramebuffer(), anim_data[nyanCatFramePos], 240 * 240);
    nyanCatFramePos++;
    if (nyanCatFramePos >= nyanCatFrameCount)
    {
        nyanCatFramePos = 0;
    }
    // Draw hours, minutes and seconds time in string format: HH:MM:SS on the top
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
    printCenteredText(gfx, buf, RGB565_WHITE, 3, 120, 50);
    flushDisplay(gfx->getFramebuffer());
}
