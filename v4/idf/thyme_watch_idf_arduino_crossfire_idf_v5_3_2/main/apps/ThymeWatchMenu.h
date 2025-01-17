#pragma once
#include "Arduino.h"
#include <framework/ThymeApp.h>
#include "Arduino_Canvas_6bit.h"

typedef struct
{
    const char *title;
    const char *subtitle;
    const uint8_t *xbm_icon;
    uint16_t icon_width;
    uint16_t icon_height;
} MenuItem;

class ThymeWatchMenu : public ThymeApp
{
public:
    ThymeWatchMenu();
    std::string appId() override;
    void onStart() override;
    void onStop() override;
    void onDraw(Arduino_Canvas_6bit *gfx) override;
    void onButtonEvent(uint8_t buttonState, uint8_t prevState) override;
    void onBackPressed() override;
    ~ThymeWatchMenu() override;

private:
    int8_t selectedItem;

    void menuUp();
    void menuDown();
    void drawMenuItem(Arduino_Canvas_6bit *gfx, uint8_t menu_index, uint16_t centerX, uint16_t centerY);
};
