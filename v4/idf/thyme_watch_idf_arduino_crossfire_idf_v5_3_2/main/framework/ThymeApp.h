#pragma once
#include <cstdint>
#include <Arduino_GFX.h>
#include <string>
#include "Arduino_Canvas_6bit.h"
class ThymeApp
{
public:
    ThymeApp(void *params) {}
    virtual std::string appId() = 0;
    virtual void onStart(Arduino_Canvas_6bit *gfx) {}
    virtual void onStop(Arduino_Canvas_6bit *gfx) {}
    /**
     * This method is called when the app is active and draws its content every frame.
     */
    virtual void onDraw(Arduino_Canvas_6bit *gfx) = 0;
    /**
     * This method is called when a button event is detected.
     */
    virtual void onButtonEvent(uint8_t buttonState, uint8_t prevState) {}
    /**
     * This method is called when the back button is pressed.
     */
    virtual void onBackPressed();
    virtual void onBackLongPressed() {}
    virtual void onUpButtonPressed() {}
    virtual void onMiddleButtonPressed() {}
    virtual void onDownButtonPressed() {}
    virtual void onScreenTouched() {}
    /**
     * The virtual destructor
     */
    virtual ~ThymeApp() {}
    virtual bool needUpdateDigitalCrown() { return false; }
    virtual void onDigitalCrownRotated(long position) {}
};
