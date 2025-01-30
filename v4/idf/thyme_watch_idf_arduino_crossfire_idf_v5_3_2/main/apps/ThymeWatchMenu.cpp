#include "ThymeWatchMenu.h"
#include <common.h>
#include "hal/hal_ls012b7dd06.h"
#include <RasterGothic18CondBold18pt7b.h>
#include "framework/app_manager.h"
#include "apps/apps.h"
#include <util/system.h>

#define TAG "ThymeWatchMenu"

using namespace Thyme;

MenuItem menu_items[] = {
    {
        .id = "sleep",
        .title = "0.Sleep",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "watch_face_selector",
        .title = "1.Watch Faces",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "music",
        .title = "2.Music",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "settings",
        .title = "3.Settings",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "usb_storage",
        .title = "4.USB Storage",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "sd_card",
        .title = "5.SD Card",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "notifications",
        .title = "6.Notifications",
        .subtitle = nullptr,
        // .xbm_icon = notifications_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "adjust_time",
        .title = "7.Adjust Time",
        .subtitle = nullptr,
        // .xbm_icon = alarm_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "alarms",
        .title = "8.Alarms",
        .subtitle = nullptr,
        // .xbm_icon = alarm_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "sensors",
        .title = "9.Sensors",
        .subtitle = nullptr,
        // .xbm_icon = alarm_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "template_app",
        .title = "10.Template App",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "bluetooth",
        .title = "11.Bluetooth",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "battery",
        .title = "12.Battery",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "factory_reset",
        .title = "13.Factory Reset",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "breakout_game",
        .title = "14.Breakout Game",
        .subtitle = nullptr,
        // .xbm_icon = pref_20x20,
        .icon_width = 20,
        .icon_height = 20,
    },
    {
        .id = "games",
        .title = "15.Games",
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

void ThymeWatchMenu::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("ThymeWatchMenu::onStart()");
}

void ThymeWatchMenu::onStop(Arduino_Canvas_6bit *gfx)
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
    // I (195400) ThymeWatchMenu: bounds = (2, -21, 70, 22)
    // I (195457) ThymeWatchMenu: bounds = (2, -21, 66, 28)
    // MY_LOG("bounds = (%d, %d, %d, %d)", x1, y1, width, height);
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
void ThymeWatchMenu::onUpButtonPressed()
{
    menuUp();
}

void ThymeWatchMenu::onMiddleButtonPressed()
{
    MY_LOG("ThymeWatchMenu::onMiddleButtonPressed()");
    auto selected = menu_items[selectedItem];
    MY_LOG("Selected: %s", selected.id);
    // switch selected id string all cases
    if (strcmp(selected.id, "watch_face_selector") == 0)
    {
        AppManager::navigateToApp<WatchFaceSelector>();
    }
    else if (strcmp(selected.id, "music") == 0)
    {
        // AppManager::navigateToApp<MusicApp>();
    }
    else if (strcmp(selected.id, "settings") == 0)
    {
        // AppManager::navigateToApp<SettingsApp>();
    }
    else if (strcmp(selected.id, "usb_storage") == 0)
    {
        AppManager::navigateToApp<UsbMscApp>();
    }
    else if (strcmp(selected.id, "sd_card") == 0)
    {
        // AppManager::navigateToApp<SDCardApp>();
    }
    else if (strcmp(selected.id, "notifications") == 0)
    {
        // AppManager::navigateToApp<NotificationsApp>();
    }
    else if (strcmp(selected.id, "adjust_time") == 0)
    {
        // AppManager::navigateToApp<AdjustTimeApp>();
    }
    else if (strcmp(selected.id, "alarms") == 0)
    {
        // AppManager::navigateToApp<AlarmsApp>();
    }
    else if (strcmp(selected.id, "sensors") == 0)
    {
        // AppManager::navigateToApp<SensorsApp>();
    }
    else if (strcmp(selected.id, "template_app") == 0)
    {
        AppManager::navigateToApp<TemplateApp>();
    }
    else if (strcmp(selected.id, "bluetooth") == 0)
    {
        // AppManager::navigateToApp<BluetoothApp>();
    }
    else if (strcmp(selected.id, "battery") == 0)
    {
        // AppManager::navigateToApp<BatteryApp>();
    }
    else if (strcmp(selected.id, "factory_reset") == 0)
    {
        // AppManager::navigateToApp<FactoryResetApp>();
    }
    else if (strcmp(selected.id, "sleep") == 0)
    {
        enter_deep_sleep();
    }
    else if (strcmp(selected.id, "breakout_game") == 0)
    {
        AppManager::navigateToApp<Game_Breakout>();
    }
    else
    {
        MY_LOG("Unknown menu item selected");
    }
}

void ThymeWatchMenu::onDownButtonPressed()
{
    menuDown();
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

void ThymeWatchMenu::onDigitalCrownRotated(long position)
{
    if (position > lastDigitalCrownPosition + DIGITAL_CROWN_STEP)
    {
        menuUp();
        lastDigitalCrownPosition = position;
    }
    else if (position < lastDigitalCrownPosition - DIGITAL_CROWN_STEP)
    {
        menuDown();
        lastDigitalCrownPosition = position;
    }
}

ThymeWatchMenu::~ThymeWatchMenu()
{
    MY_LOG("ThymeWatchMenu::~ThymeWatchMenu()");
}
