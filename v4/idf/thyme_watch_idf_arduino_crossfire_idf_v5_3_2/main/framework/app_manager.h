#pragma once

#include <Arduino.h>
#include <hal/hal_ls012b7dd06.h>
#include "ThymeApp.h"

namespace Thyme::AppManager
{
    extern ThymeApp *currentApp;
    extern uint8_t frame_buffer[LCD_WIDTH * LCD_HEIGHT];
    extern Arduino_Canvas_6bit *gfx;
    extern uint8_t prevButtonState;
    extern unsigned long lastInteractTime;

    template <typename T>
    void navigateToApp(void *params = nullptr)
    {
        static_assert(std::is_base_of<ThymeApp, T>::value, "T must be a subclass of ThymeApp");
        if (currentApp != nullptr)
        {
            currentApp->onStop(gfx);
            delete currentApp;
        }
        currentApp = new T(params);
        currentApp->onStart(gfx);
    }

    void checkNotifyButtonEvent(uint8_t buttonState);
    void checkNotifyBackButton(bool backPressed);
    void notifyBackButtonPressed();
    void notifyBackButtonLongPressed();

    void checkNotifyDigitalCrown(long position);
    bool idleNeedDeepSleep();

    inline bool buttonUpPressed(uint8_t state)
    {
        return !(state & 0b001);
    }
    inline bool buttonMiddlePressed(uint8_t state)
    {
        return !(state & 0b010);
    }
    inline bool buttonDownPressed(uint8_t state)
    {
        return !(state & 0b100);
    }
    inline Arduino_Canvas_6bit *getCanvas()
    {
        return gfx;
    }
    inline uint8_t *getBuffer()
    {
        return frame_buffer;
    }
    inline bool isScreenTouched(uint8_t state)
    {
        return state & (1 << 7);
    }
}
