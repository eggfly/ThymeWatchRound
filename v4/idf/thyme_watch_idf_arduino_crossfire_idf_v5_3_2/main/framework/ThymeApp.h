#pragma once
#include <cstdint>
#include <Arduino_GFX.h>
#include <string>
#include "Arduino_Canvas_6bit.h"
class ThymeApp
{
public:
    virtual std::string appId() = 0;
    virtual void onStart(Arduino_Canvas_6bit *gfx) {}
    virtual void onStop(Arduino_Canvas_6bit *gfx) {}
    virtual void onDraw(Arduino_Canvas_6bit *gfx) = 0;
    virtual void onButtonEvent(uint8_t buttonState, uint8_t prevState) {}
    virtual void onBackPressed();
    virtual void onUpButtonPressed() {}
    virtual void onMiddleButtonPressed() {}
    virtual void onDownButtonPressed() {}
    virtual void onScreenTouched() {}
    virtual ~ThymeApp() {}
    virtual bool needUpdateDigitalCrown() { return false; }
    virtual void onDigitalCrownRotated(long position) {}
};
