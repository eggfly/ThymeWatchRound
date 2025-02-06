#pragma once

#include "Arduino.h"
#include <framework/ThymeApp.h>
#include "Arduino_Canvas_6bit.h"

typedef struct
{
    const char *id;
    const char *title;
    const char *subtitle;
    const uint8_t *xbm_icon;
    uint16_t icon_width;
    uint16_t icon_height;
} MenuItem;

class ThymeWatchMenu : public ThymeApp
{
public:
    ThymeWatchMenu(void *params = nullptr);
    std::string appId() override;
    void onStart(Arduino_Canvas_6bit *gfx) override;
    void onStop(Arduino_Canvas_6bit *gfx) override;
    void onDraw(Arduino_Canvas_6bit *gfx) override;
    void onUpButtonPressed() override;
    void onMiddleButtonPressed() override;
    void onDownButtonPressed() override;
    bool needUpdateDigitalCrown() override { return true; }
    void onDigitalCrownRotated(long position) override;
    ~ThymeWatchMenu() override;

private:
    const long DIGITAL_CROWN_STEP = 15;
    int8_t selectedItem;
    long lastDigitalCrownPosition;
    void menuUp();
    void menuDown();
    void drawMenuItem(Arduino_Canvas_6bit *gfx, uint8_t menu_index, uint16_t centerX, uint16_t centerY);
};
