#pragma once

#include "Arduino.h"
#include <framework/ThymeApp.h>
#include "Arduino_Canvas_6bit.h"

class Game_Breakout : public ThymeApp
{
public:
    Game_Breakout(void* params);
    std::string appId() override;
    void onStart(Arduino_Canvas_6bit *gfx) override;
    void onStop(Arduino_Canvas_6bit *gfx) override;
    void onDraw(Arduino_Canvas_6bit *gfx) override;
    void onUpButtonPressed() override;
    void onMiddleButtonPressed() override;
    void onDownButtonPressed() override;
    bool needUpdateDigitalCrown() override { return true; }
    void onDigitalCrownRotated(long position) override;
    ~Game_Breakout() override;

private:
    long lastDigitalCrownPosition = 0;
    // 二分之根号2 * 120 = 84.85
    const int16_t WIDTH_HEIGHT = 85 * 2;
    const int16_t BALL_RADIUS = 3;
    unsigned long lastTime = 0;
    double ballPosX = 120;
    double ballPosY = 120;
    double ballSpeedX = 8; // pixels per second
    double ballSpeedY = 8; // pixels per second
};
