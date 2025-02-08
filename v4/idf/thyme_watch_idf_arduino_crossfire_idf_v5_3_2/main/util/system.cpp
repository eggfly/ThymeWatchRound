#include "system.h"

#include <SD_MMC.h>
#include "common.h"
#include "driver/gpio.h"
#include "hal/hal_pca6408a.h"
#include "hal/hal_battery_fuel_gauge.h"
#include "hal/hal_ls012b7dd06.h"
#include "hal/hal_digital_crown.h"
#include "config.h"

#define TAG "System"

bool backlightOn = true;

void enableSensorPower()
{
    pinMode(9, OUTPUT);
    digitalWrite(9, HIGH);
}

void disableSensorPower()
{
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
}

void myShortToneAsync()
{
    tone(BUZZER_PIN, 8000, 30);
    MY_LOG("short tone started");
}

void myShortToneSync()
{
    tone(BUZZER_PIN, 8000, 30);
    delay(30);
    noTone(BUZZER_PIN);
}

void enter_deep_sleep()
{
    MY_LOG("Enter deep sleep");
    disableSensorPower();
    // Turn off backlight
    // hibernate_fuel_gauge() 节约了0.02mA0.50mA -> 0.48mA, 去掉5v boost 变成 0.46mA
    hibernate_fuel_gauge();

    // pca6408a.setDigital(PCA6408A_IO3, LOW);
    pca6408a.setDirection(PCA6408A_IO7_INPUT |
                          PCA6408A_IO6_INPUT |
                          PCA6408A_IO5_INPUT |
                          PCA6408A_IO4_INPUT |
                          PCA6408A_IO3_INPUT |
                          PCA6408A_IO2_INPUT |
                          PCA6408A_IO1_INPUT |
                          PCA6408A_IO0_INPUT);
    if (ENABLE_POWER_AND_SLEEP_BUZZER)
    {
        myShortToneSync();
        delay(100);
        myShortToneSync();
    }

    gpio_hold_en((gpio_num_t)VCOM_PIN);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_PIN, 0); // 0 表示低电平唤醒
    // esp_sleep_enable_timer_wakeup(SLEEP_DURATION * 1000000ULL);
    esp_deep_sleep_start();
}

void toggleBacklight()
{
    backlightOn = !backlightOn;
    pca6408a.setDigital(PCA6408A_IO3, backlightOn ? HIGH : LOW);
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    MY_LOG("Listing directory: %s", dirname);
    File root = fs.open(dirname);
    if (!root)
    {
        MY_LOG("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        MY_LOG("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            MY_LOG("  DIR : %s", file.name());
            if (levels)
            {
                listDir(fs, file.path(), levels - 1);
            }
        }
        else
        {
            MY_LOG("  FILE: %s  SIZE: %u", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

void init_sdmmc()
{
    if (!SD_MMC.setPins(CONFIG_EXAMPLE_PIN_CLK,
                        CONFIG_EXAMPLE_PIN_CMD,
                        CONFIG_EXAMPLE_PIN_D0,
                        CONFIG_EXAMPLE_PIN_D1,
                        CONFIG_EXAMPLE_PIN_D2,
                        CONFIG_EXAMPLE_PIN_D3))
    {
        MY_LOG("Failed to set SD_MMC pins");
        return;
    }
    if (!SD_MMC.begin("/sdcard", false, true, BOARD_MAX_SDMMC_FREQ))
    {
        MY_LOG("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE)
    {
        MY_LOG("No SD_MMC card attached");
        return;
    }

    MY_LOG("SD_MMC Card Type: ");
    if (cardType == CARD_MMC)
    {
        MY_LOG("MMC");
    }
    else if (cardType == CARD_SD)
    {
        MY_LOG("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        MY_LOG("SDHC");
    }
    else
    {
        MY_LOG("UNKNOWN");
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    MY_LOG("SD_MMC Card Size: %lluMB", cardSize);
    listDir(SD_MMC, "/", 0);
}

void enableSensorPowerIdf()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << 9, // 选择 GPIO9
        .mode = GPIO_MODE_OUTPUT,  // 配置为输出模式
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE // 中断模式关
    };
    gpio_config(&io_conf);

    // 拉高 GPIO9
    gpio_set_level(GPIO_NUM_9, 1);
    // int pin9 = digitalRead(9);
    // ESP_LOGI(TAG, "pin9 is %d", pin9);
}

void my_esp_restart()
{
    reset_pat9125();
    esp_restart();
}
