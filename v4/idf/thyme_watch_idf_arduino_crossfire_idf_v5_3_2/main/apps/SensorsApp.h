#pragma once

#include "Arduino.h"
#include <framework/ThymeApp.h>
#include "Arduino_Canvas_6bit.h"

class SensorsApp : public ThymeApp
{
public:
    SensorsApp(void* params);
    std::string appId() override;
    void onStart(Arduino_Canvas_6bit *gfx) override;
    void onStop(Arduino_Canvas_6bit *gfx) override;
    void onDraw(Arduino_Canvas_6bit *gfx) override;
    void onUpButtonPressed() override;
    void onMiddleButtonPressed() override;
    void onDownButtonPressed() override;
    ~SensorsApp() override;

private:
};
