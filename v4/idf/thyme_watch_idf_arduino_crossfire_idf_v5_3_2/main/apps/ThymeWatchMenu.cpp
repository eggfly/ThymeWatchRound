#include "ThymeWatchMenu.h"
#include <common.h>
#include "hal/hal_ls012b7dd06.h"
#include <RasterGothic18CondBold18pt7b.h>

#define TAG "ThymeWatchMenu"

MenuItem menu_items[] = {
    {
        .title = "Watch Faces",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Music",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Settings",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "USB Storage",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "SD Card",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Notifications",
        .subtitle = nullptr,
        // .xbm_icon = notifications_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Adjust Time",
        .subtitle = nullptr,
        // .xbm_icon = alarm_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Alarms",
        .subtitle = nullptr,
        // .xbm_icon = alarm_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Sensors",
        .subtitle = nullptr,
        // .xbm_icon = alarm_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Bluetooth",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Battery",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Factory Reset",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .title = "Sleep",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
};

const size_t menu_items_count =
    sizeof(menu_items) / sizeof(menu_items[0]);

ThymeWatchMenu::ThymeWatchMenu()
{
    MY_LOG("ThymeWatchMenu::ThymeWatchMenu()");
    selectedItem = 0;
}

std::string ThymeWatchMenu::appId()
{
    return "menu";
}

void ThymeWatchMenu::onStart()
{
    MY_LOG("ThymeWatchMenu::onStart()");
}

void ThymeWatchMenu::onStop()
{
    MY_LOG("ThymeWatchMenu::onStop()");
}

void ThymeWatchMenu::drawMenuItem(Arduino_Canvas_6bit *gfx, uint8_t menu_index, uint16_t centerX, uint16_t centerY)
{
    uint16_t width, height;
    int16_t x1, y1;
    bool highlighted = menu_index == selectedItem;
    MenuItem item = menu_items[menu_index];
    gfx->setFont(&RasterGothic18CondBold18pt7b);
    gfx->setTextSize(highlighted ? 1 : 1);
    gfx->setTextColor(highlighted ? RGB565_BLACK : RGB565_WHITE);
    gfx->getTextBounds(item.title, 0, 0, &x1, &y1, &width, &height);
    MY_LOG("bounds = (%d, %d, %d, %d)", x1, y1, width, height);
    if (highlighted)
    {
        gfx->fillRoundRect(centerX - width / 2 - 5, centerY - height / 2 - 5, width + 10, height + 10, 5, RGB565_WHITE);
    }
    // cursor position is left bottom corner
    // gfx->setCursor(centerX - width / 2, centerY - height / 2);
    gfx->setCursor(centerX - width / 2, centerY + height / 2);
    gfx->print(item.title);
}

void ThymeWatchMenu::onDraw(Arduino_Canvas_6bit *gfx)
{
    gfx->fillScreen(RGB565_BLACK);
    gfx->setTextColor(RGB565_WHITE);
    this->drawMenuItem(gfx, selectedItem, 120, 120);
    // draw before 2 items and after 2 items in two loops
    // call drawMenuItem() in loop
    for (int i = 0; i < 2; i++) // before
    {
        int8_t menu_index = (menu_items_count + selectedItem - i - 1) % menu_items_count;
        if (menu_index >= 0)
        {
            this->drawMenuItem(gfx, menu_index, 120, 120 - 40 * (i + 1));
        }
    }
    for (int i = 0; i < 2; i++) // after
    {
        int8_t menu_index = (selectedItem + i + 1) % menu_items_count;
        if (menu_index < menu_items_count)
        {
            this->drawMenuItem(gfx, menu_index, 120, 120 + 40 * (i + 1));
        }
    }

    flushDisplay(gfx->getFramebuffer());
}

void ThymeWatchMenu::onButtonEvent(uint8_t buttonState, uint8_t prevState)
{
    MY_LOG("ThymeWatchMenu::onButtonEvent(%d, %d)", buttonState, prevState);
    bool btnUp = buttonState & 0b001;
    bool btnMid = buttonState & 0b010;
    bool btnDown = buttonState & 0b100;
    bool btnUpPrev = prevState & 0b001;
    bool btnMidPrev = prevState & 0b010;
    bool btnDownPrev = prevState & 0b100;
    if (btnUp && !btnUpPrev)
    {
        MY_LOG("Up pressed");
        menuUp();
    }
    if (btnMid && !btnMidPrev)
    {
        MY_LOG("Mid pressed");
    }
    if (btnDown && !btnDownPrev)
    {
        MY_LOG("Down pressed");
        menuDown();
    }
}

void ThymeWatchMenu::menuUp()
{
    selectedItem--;
    if (selectedItem < 0)
    {
        selectedItem = menu_items_count - 1;
    }
    MY_LOG("ThymeWatchMenu::menuUp()");
}

void ThymeWatchMenu::menuDown()
{
    selectedItem++;
    if (selectedItem >= menu_items_count)
    {
        selectedItem = 0;
    }
    MY_LOG("ThymeWatchMenu::menuDown()");
}

ThymeWatchMenu::~ThymeWatchMenu()
{
    MY_LOG("ThymeWatchMenu::~ThymeWatchMenu()");
}

void ThymeWatchMenu::onBackPressed()
{
    MY_LOG("ThymeWatchMenu::onBackPressed()");
    // navigateToApp<ThymeWatchFace>();
}
