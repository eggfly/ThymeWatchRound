#pragma once
#include <cstdint>
#include <Arduino_GFX.h>
#include <string>
#include "Arduino_Canvas_6bit.h"

class ThymeApp
{
public:
    virtual std::string appId() = 0;
    virtual void onStart() = 0;
    virtual void onStop() = 0;
    virtual void onDraw(Arduino_Canvas_6bit *gfx) = 0;
    virtual void onButtonEvent(uint8_t buttonState, uint8_t prevState) = 0;
    virtual void onBackPressed() = 0;
    virtual ~ThymeApp() {}
};
