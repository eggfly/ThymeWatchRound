#include "UsbMscApp.h"
#include <common.h>
#include <config.h>
#include <hal/hal_ls012b7dd06.h>

#include <SD_MMC.h>
#include <RasterGothic18CondBold18pt7b.h>

#include "util/ui.h"
#include <apps/apps.h>
#include "framework/app_manager.h"

#include "tinyusb.h"
#include "tusb_msc_storage.h"
#include "tusb_msc.h"
#include <util/system.h>
#include <Arduino_GFX.h>

#define TAG "UsbMscApp"

using namespace Thyme;

UsbMscApp::UsbMscApp()
{
    MY_LOG("UsbMscApp::UsbMscApp()");
}

std::string UsbMscApp::appId()
{
    return "usb_msc";
}

void UsbMscApp::onStart(Arduino_Canvas_6bit *gfx)
{
    // Should call this overload to avoid crash.
    gfx->setFont((const GFXfont *)nullptr);
    gfx->setTextSize(1);
    MY_LOG("UsbMscApp::onStart()");
    // canvas->fillScreen(COLOR_RED);
    gfx->fillScreen(RGB565_RED);
    printCenteredText(gfx, "YOU'RE NOW IN", COLOR_WHITE, 2, 120, 100);
    printCenteredText(gfx, "USB MSC MODE!", COLOR_WHITE, 2, 120, 120);
    printCenteredText(gfx, "--Have Fun!--", COLOR_WHITE, 2, 120, 140);
    flushDisplay(gfx->getFramebuffer());
    SD_MMC.end();
    MY_LOG("Enter USB MSC");
    enter_usb_msc();
    waitForRebootKeyForever();
    /* Simply restart make usb not vailable, dont know why */
}

void UsbMscApp::onDraw(Arduino_Canvas_6bit *gfx)
{
    gfx->fillScreen(RGB565_BLACK);
    printCenteredText(gfx, "UsbMscApp", RGB565_WHITE, 1, 120, 120);
    flushDisplay(gfx->getFramebuffer());
}

void UsbMscApp::onUpButtonPressed()
{
    MY_LOG("UsbMscApp::onUpButtonPressed()");
}

void UsbMscApp::onMiddleButtonPressed()
{
    MY_LOG("UsbMscApp::onMiddleButtonPressed()");
}

void UsbMscApp::onDownButtonPressed()
{
    MY_LOG("UsbMscApp::onDownButtonPressed()");
}

UsbMscApp::~UsbMscApp()
{
    MY_LOG("UsbMscApp::~UsbMscApp()");
}

void UsbMscApp::waitForRebootKeyForever()
{
    /* Quit when key pressed */
    gpio_reset_pin(REBOOT_PIN);
    gpio_set_direction(REBOOT_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(REBOOT_PIN, GPIO_PULLUP_ONLY);
    while (1)
    {
        /* If pressed */
        if (gpio_get_level(REBOOT_PIN) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(20));
            if (gpio_get_level(REBOOT_PIN) == 0)
            {
                MY_LOG("Reboot button pressed");
                /* Wait released */
                while (gpio_get_level(REBOOT_PIN) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }

                MY_LOG("Rebooting...");
                tinyusb_msc_storage_unmount();
                tinyusb_msc_storage_deinit();
                vTaskDelay(200);
                my_esp_restart();
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
