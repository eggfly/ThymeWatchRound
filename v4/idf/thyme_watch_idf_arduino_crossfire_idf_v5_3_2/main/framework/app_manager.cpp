#include "app_manager.h"
#include "common.h"
#include <config.h>

#define TAG "AppManager"
namespace Thyme::AppManager
{
    ThymeApp *currentApp = nullptr;
    uint8_t prevButtonState;
    bool prevBackPressed = false;
    uint8_t frame_buffer[LCD_WIDTH * LCD_HEIGHT];
    Arduino_Canvas_6bit *gfx;
    unsigned long lastInteractTime = 0;

    void checkNotifyButtonEvent(uint8_t buttonState)
    {
        if (currentApp != nullptr)
        {
            // MY_LOG("checkNotifyButtonEvent(%d), prevButtonState: %d", buttonState, prevButtonState);
            if (prevButtonState != buttonState)
            {
                currentApp->onButtonEvent(buttonState, prevButtonState);
                lastInteractTime = millis();
            }
            bool upButtonPressed = buttonUpPressed(buttonState);
            bool midButtonPressed = buttonMiddlePressed(buttonState);
            bool downButtonPressed = buttonDownPressed(buttonState);
            bool isTouched = isScreenTouched(buttonState);
            bool upButtonPrev = buttonUpPressed(prevButtonState);
            bool midButtonPrev = buttonMiddlePressed(prevButtonState);
            bool downButtonPrev = buttonDownPressed(prevButtonState);
            bool isTouchedPrev = isScreenTouched(prevButtonState);
            if (upButtonPressed && !upButtonPrev)
            {
                currentApp->onUpButtonPressed();
            }
            if (midButtonPressed && !midButtonPrev)
            {
                currentApp->onMiddleButtonPressed();
            }
            if (downButtonPressed && !downButtonPrev)
            {
                currentApp->onDownButtonPressed();
            }
            if (isTouched && !isTouchedPrev)
            {
                currentApp->onScreenTouched();
            }
        }
        prevButtonState = buttonState;
    }

    void checkNotifyBackButton(bool backPressed)
    {
        if (currentApp != nullptr)
        {
            if (backPressed && !prevBackPressed)
            {
                currentApp->onBackPressed();
            }
        }
        if (prevBackPressed != backPressed)
        {
            lastInteractTime = millis();
        }
        prevBackPressed = backPressed;
    }

    void checkNotifyDigitalCrown(long position)
    {
        if (currentApp != nullptr && currentApp->needUpdateDigitalCrown())
        {
            currentApp->onDigitalCrownRotated(position);
        }
    }

    bool idleNeedDeepSleep()
    {
        return millis() - lastInteractTime > IDLE_DEEP_SLEEP_TIME;
    }
}
