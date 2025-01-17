#include "app_manager.h"

namespace Thyme::AppManager
{
    ThymeApp *currentApp = nullptr;
    uint8_t prevButtonState;
    void checkNotifyButtonEvent(uint8_t buttonState)
    {
        if (prevButtonState != buttonState)
        {
            if (currentApp != nullptr)
            {
                currentApp->onButtonEvent(buttonState, prevButtonState);
            }
            prevButtonState = buttonState;
        }
    }
    void onBackPressed()
    {
        if (currentApp != nullptr)
        {
            currentApp->onBackPressed();
        }
    }
}
