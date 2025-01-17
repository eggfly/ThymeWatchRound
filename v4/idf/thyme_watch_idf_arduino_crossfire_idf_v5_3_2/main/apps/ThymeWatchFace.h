#pragma once
#include "Arduino.h"
#include <framework/ThymeApp.h>
#include "Arduino_Canvas_6bit.h"

class ThymeWatchFace : public ThymeApp
{
public:
    ThymeWatchFace();
    std::string appId() override;
    void onStart() override;
    void onStop() override;
    void onDraw(Arduino_Canvas_6bit *gfx) override;
    void onButtonEvent(uint8_t buttonState, uint8_t prevState) override;
    void onBackPressed() override;
    ~ThymeWatchFace() override;

private:
};
