#pragma once

#include <Arduino.h>

#include "ThymeApp.h"

namespace Thyme::AppManager
{
    extern ThymeApp *currentApp;

    template <typename T>
    void navigateToApp()
    {
        static_assert(std::is_base_of<ThymeApp, T>::value, "T must be a subclass of ThymeWatchApp");
        if (currentApp != nullptr)
        {
            currentApp->onStop();
            delete currentApp;
        }
        currentApp = new T();
        currentApp->onStart();
    }

    extern uint8_t prevButtonState;

    void checkNotifyButtonEvent(uint8_t buttonState);
    void onBackPressed();
}
