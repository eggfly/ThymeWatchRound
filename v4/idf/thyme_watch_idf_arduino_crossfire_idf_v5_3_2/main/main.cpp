#include <stdio.h>
#include <Arduino.h>
#include <Wire.h>
#include <SD_MMC.h>

#include "driver/gpio.h"
#include "tusb_msc.h"
#include "esp_task_wdt.h"

// #include <Adafruit_GFX.h>
#include <Arduino_GFX_Library.h>
#include "Arduino_Canvas_6bit.h"

#include "util/memory.h"

#include "hal/hal_pca6408a.h"
#include "hal/hal_ls012b7dd06.h"
#include "hal/hal_digital_crown.h"
#include "hal/hal_battery_fuel_gauge.h"
#include "hal/hal_imu.h"
#include "hal/hal_pressure_sensor.h"
#include "hal/hal_rtc.h"
#include "hal/hal_va_meter.h"
#include "hal/hal_ble_server.h"

#include "config.h"
#include "common.h"
#include "resources/bloxx.h"
#include "resources/jiaming2.h"
#include "resources/jiaming3.h"
#include "resources/casio_yellow.h"
#include "resources/music_player.h"

#include "apps/apps.h"
#include "framework/app_manager.h"

// #include "Fonts/FreeMono12pt7b.h"
// #include "Fonts/FreeMonoBold9pt7b.h"
// #include "Fonts/FreeMonoBold12pt7b.h"
// #include "Fonts/FreeMonoBold18pt7b.h"

#include "resources/anim_data_12_frames.h"
#include "resources/lcd_digits.h"
#include <LECO_1976_Regular1pt7b.h>
#include <RasterGothic18CondBold18pt7b.h>
#include "tusb_msc_storage.h"

using namespace Thyme;

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED != 1
#error "USB Serial JTAG is not enabled"
#endif

#ifdef CONFIG_ESP_CONSOLE_UART_DEFAULT
#error "UART console cannot enable, because USB Serial JTAG is enabled"
#endif

#if CONFIG_TINYUSB_MSC_BUFSIZE != 8192
#error "MSC FIFO size is not 8192"
#endif

#if CONFIG_TINYUSB_MSC_ENABLED != 1
#error "MSC is not enabled"
#endif

static const char *TAG = "Thyme";

uint8_t frame_buffer[LCD_WIDTH * LCD_HEIGHT];

uint8_t screen_buffer[LCD_WIDTH * LCD_HEIGHT];

void wait_for_key_forever()
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
                esp_restart();
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// GFXcanvas8 *canvas = new GFXcanvas8(240, 240);

Arduino_Canvas_6bit *gfx;

void printCenteredText(const char *text, uint8_t color, uint8_t size, uint16_t centerX, uint16_t centerY)
{
    // canvas->setTextColor(color);
    // canvas->setTextSize(size);
    uint16_t width, height;
    int16_t x1, y1;
    // canvas->getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
    // canvas->setCursor(centerX - width / 2, centerY - height / 2);
    // canvas->print(text);

    gfx->setTextSize(size);
    // gfx->setFont(nullptr);
    gfx->setTextColor(RGB565_WHITE);
    gfx->getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
    gfx->setCursor(centerX - width / 2, centerY - height / 2);
    gfx->print(text);
}

uint8_t _16_colors[16] = {
    COLOR_BLACK,
    COLOR_WHITE,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_YELLOW,
    COLOR_GRAY,
    COLOR_DARK_RED,
    COLOR_DARK_GREEN,
    COLOR_DARK_BLUE,
    COLOR_ORANGE,
    COLOR_PURPLE,
    COLOR_BROWN,
    COLOR_LIGHT_GRAY,
};

void testFillRainbow()
{
    for (int i = 0; i < 16; i++)
    {
        gfx->fillCircle(120, 120, 120 - i * 7, _16_colors[i]);
        // canvas->fillCircle(120, 120, 120 - i * 7, _16_colors[i]);
    }
}

bool backlightOn = true;

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

// Primary I2C:
// Found address: 32 (0x20)  -> PCA6408A (IO expander)
// Found address: 50 (0x32)  -> INS5699S (RTC)
// Found address: 54 (0x36)  -> MAX17048 Fuel Gauge
// Found address: 105 (0x69) -> 6-Axis BMI270(BMI2_I2C_SEC_ADDR)
// Secondary I2C:
// Found address: 64 (0x40)  -> INA219
// Found address: 68 (0x44)  -> OPT3001
// Found address: 69 (0x45)  -> SHT30
// Found address: 117 (0x75) -> PAT9125
// Found address: 118 (0x76) -> BMP280
void Scanner(const char *headerText, TwoWire &wire)
{
    MY_LOG("%s I2C scanner. Scanning ...", headerText);
    byte count = 0;

    for (byte i = 8; i < 120; i++)
    {
        wire.beginTransmission(i);       // Begin I2C transmission Address (i)
        if (wire.endTransmission() == 0) // Receive 0 = success (ACK response)
        {
            MY_LOG("Found address: %d (0x%02X)", i, i);
            count++;
        }
    }
    MY_LOG("Found %d device(s).", count);
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

void initArduinoGFX()
{
    gfx = new Arduino_Canvas_6bit(240, 240, nullptr);
    // I (409) Memory: Total heap size: 140572 bytes, Max alloc size: 57344 bytes
    // I (409) Memory: Free memory: 119252 bytes, Total memory: 140572 bytes
    // I (409) Memory: No PSRAM found
    // I (410) Thyme: Failed to initialize GFX, no memory?
    print_memory_info();
    gfx->setCustomFrameBuffer(frame_buffer);
    // bool result = gfx->begin();
    // if (!result)
    // {
    //     MY_LOG("Failed to initialize GFX, no memory?");
    //     while (1)
    //         ;
    // }
    gfx->fillScreen(COLOR_WHITE);
    gfx->setUTF8Print(true); // enable UTF8 support for the Arduino print() function
    gfx->setFont(u8g2_font_unifont_t_chinese);
}

tmElements_t tm;
va_meter_t va_meter_data;
fuel_gauge_t fuel_gauge_data;
imu_t imu_data;
uint16_t nyanCatFramePos = 0;

void renderNyanCat()
{
    uint16_t nyanCatFrameCount = sizeof(anim_data) / sizeof(anim_data[0]);
    MY_LOG("nyanCatFrameCount: %d", nyanCatFrameCount);
    memcpy(gfx->getFramebuffer(), anim_data[nyanCatFramePos], 240 * 240);
    nyanCatFramePos++;
    if (nyanCatFramePos >= nyanCatFrameCount)
    {
        nyanCatFramePos = 0;
    }
}

void renderCasioWatchFace()
{
    memcpy(gfx->getFramebuffer(), casio_yellow, 240 * 240);
    // 小时的第1位
    gfx->drawBitmap(46, 114, digits_20x44[tm.Hour / 10], 20, 44, RGB565_BLACK);
    // 小时的第2位
    gfx->drawBitmap(71, 114, digits_20x44[tm.Hour % 10], 20, 44, RGB565_BLACK);
    // 分钟的第1位
    gfx->drawBitmap(107, 114, digits_20x44[tm.Minute / 10], 20, 44, RGB565_BLACK);
    // 分钟的第2位
    gfx->drawBitmap(132, 114, digits_20x44[tm.Minute % 10], 20, 44, RGB565_BLACK);
    // 秒钟的第1位
    gfx->drawBitmap(160, 129, digits_14x29[tm.Second / 10], 14, 29, RGB565_BLACK);
    // 秒钟的第2位
    gfx->drawBitmap(178, 129, digits_14x29[tm.Second % 10], 14, 29, RGB565_BLACK);
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

    gpio_hold_en((gpio_num_t)VCOM_PIN);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_PIN, 0); // 0 表示低电平唤醒
    // esp_sleep_enable_timer_wakeup(SLEEP_DURATION * 1000000ULL);
    esp_deep_sleep_start();
}

inline __attribute__((always_inline)) uint8_t *getBuffer()
{
    return gfx->getFramebuffer();
}

void toggleBacklight()
{
    backlightOn = !backlightOn;
    pca6408a.setDigital(PCA6408A_IO3, backlightOn ? HIGH : LOW);
}

bool gPrevTouched = false;

extern "C"
{
    void app_main(void)
    {
        MY_LOG("Now app_main() called, time is %ld", millis());
        initArduino();
        MY_LOG("initArduino() called.");
        // USB CDC doesn't need a baud rate
        Serial.begin();
        Serial.println("Hello idf arduino!");
        // Serial.begin(115200);
        // esp_task_wdt_add(NULL); // add current task to WDT watch
        Wire.end();
        // Enable sensors power
        enableSensorPower();
        // Initialize I2C
        if (!Wire.begin(ULP_I2C_SDA, ULP_I2C_SCL, I2C_FREQUENCY))
        {
            MY_LOG("I2C initialization failed!");
            while (1)
                ;
        }
        else
        {
            MY_LOG("I2C initialization success!");
        }
        // Initialize 2nd I2C
        if (Wire1.begin(_2ND_I2C_SDA, _2ND_I2C_SCL, I2C_FREQUENCY) == 0)
        {
            MY_LOG("I2C1 initialization failed!");
            while (1)
                ;
        }
        else
        {
            MY_LOG("I2C1 initialization success!");
        }

        initRTC();

        // Init backlight and input/output expander
        initPCA6408A();

        // Init screen
        initScreenPWMPins();
        initScreenOutputPins();
        resetDisplay();
        initDisplay();
        initArduinoGFX();

        MY_LOG("Screen initialized");
        auto startTime = millis();
        for (int loops = 0; loops < 3; loops++)
        {
            for (int i = 0; i < 16; i++)
            {
                memset(screen_buffer, _16_colors[i], sizeof(screen_buffer));
                flushDisplay(screen_buffer);
            }
        }
        // Init acce and gyro meters.
        init_ble_server();
        // init_imu();
        // Init va meter
        init_va_meter();
        // Init fuel gauge
        init_fuel_gauge();
        // Init digital crown
        init_pat9125();
        init_pressure_sensor();
        init_sdmmc();

        Scanner("===  Primary I2C  === ", Wire);
        Scanner("=== Secondary I2C === ", Wire1);

        testFillRainbow();
        flushDisplay(getBuffer());
        printCenteredText("Hello World!", COLOR_BLACK, 3, 120, 120);
        printCenteredText("UP MassStorage ->", COLOR_BLACK, 2, 120, 60);
        flushDisplay(getBuffer());
        MY_LOG("Screen flushed frames, cost %ld ms", millis() - startTime);
        // memcpy(image_bloxx, canvas->getBuffer(), sizeof(image_bloxx));
        flushDisplay((uint8_t *)image_jiaming3);
        if (USE_CONST_VCOM_NOT_PWM)
        {
            initScreenPWMPins();
        }
        else
        {
            initVcomLedc();
        }
        uint8_t inputs;
        if (pca6408a.readInput(&inputs))
        {
            MY_LOG("Inputs: %d", inputs);
            bool pin0 = !(inputs & 0b001);
            bool pin1 = !(inputs & 0b010);
            bool pin2 = !(inputs & 0b100);
            if (pin0 || pin1 || pin2)
            {
                MY_LOG("Enter USB MSC");
                enter_usb_msc();
                wait_for_key_forever();
                /* Simply restart make usb not vailable, dont know why */
                // pmu.powerOff();
                return;
            }
        }

        auto sensorUpdateTime = 0;
        auto rtcUpdateTime = 0;
        char buf[256];
        char power_buf[256];
        while (true)
        {
            if (Serial.available() > 0)
            {
                // 一旦缓冲区内有可读数据，则读取1字节
                int incomingByte = Serial.read();
                // 以十进制打印读取到的字节
                MY_LOG("Received: %d", incomingByte);
            }
            uint8_t inputs;
            if (pca6408a.readInput(&inputs))
            {
                // MY_LOG("Inputs: %d", inputs);
                bool pin0 = !(inputs & 0b001);
                bool pin1 = !(inputs & 0b010);
                bool pin2 = !(inputs & 0b100);
                bool touched = inputs & (1 << 7);
                if (AppManager::currentApp != nullptr)
                {
                    AppManager::checkNotifyButtonEvent(inputs);
                }
                else
                {
                    if (touched && !gPrevTouched)
                    {
                        // toggleBacklight();
                        MY_LOG("Touched!");
                        // delay(200);
                        // toggleBacklight();
                    }
                    gPrevTouched = touched;
                    MY_LOG("Inputs: %d, pin0: %d, pin1: %d, pin2: %d, touched: %d", inputs, pin0, pin1, pin2, touched);
                    if (pin0)
                    {
                        // canvas->fillScreen(COLOR_RED);
                        gfx->fillScreen(RGB565_RED);
                        printCenteredText("YOU'RE NOW IN", COLOR_WHITE, 2, 120, 100);
                        printCenteredText("USB MSC MODE!", COLOR_WHITE, 2, 120, 120);
                        printCenteredText("--Have Fun!--", COLOR_WHITE, 2, 120, 140);
                        flushDisplay(getBuffer());
                        SD_MMC.end();
                        MY_LOG("Enter USB MSC");
                        enter_usb_msc();
                        wait_for_key_forever();
                        /* Simply restart make usb not vailable, dont know why */
                        // pmu.powerOff();
                        return;
                    }
                    if (pin1)
                    {
                        Thyme::AppManager::navigateToApp<ThymeWatchMenu>();
                        // toggleBacklight();
                    }
                    if (pin2)
                    {
                        enter_deep_sleep();
                    }
                }
            }
            if (millis() - sensorUpdateTime > SENSOR_UPDATE_INTERVAL)
            {
                pressure_t pressure_data = read_pressure();
                snprintf(buf, sizeof(buf), "\n T: %.3f C\n P: %.3f Pa\n A: %.3f m",
                         pressure_data.temperature,
                         pressure_data.pressure,
                         pressure_data.altitude);
                tm = readRTC();
                printDateTime(tm);
                va_meter_data = read_va_meter();
                print_va_meter(va_meter_data);
                long scroll_pos = read_pat9125();
                MY_LOG("Scroll pos: %ld", scroll_pos);
                // snprintf(va_buf, sizeof(va_buf), "Bus Voltage: %.3f V, Shunt Voltage: %.2f mV, Load Voltage: %.2f V, Current: %.2f mA, Power: %.2f mW", va_meter_data.busvoltage, va_meter_data.shuntvoltage, va_meter_data.loadvoltage, va_meter_data.current_mA, va_meter_data.power_mW);
                // snprintf(va_buf, sizeof(va_buf), "%.3fV/%.2fmA", va_meter_data.loadvoltage, va_meter_data.current_mA);
                // MY_LOG("%s", va_buf);
                read_fuel_gauge(&fuel_gauge_data);
                print_fuel_gauge(fuel_gauge_data);
                snprintf(power_buf, sizeof(power_buf), "%.2fV %.2fmA %.1f%%", fuel_gauge_data.cellVoltage, va_meter_data.current_mA, fuel_gauge_data.cellPercent);
                // bool got_imu_data = read_imu(&imu_data);
                // if (got_imu_data)
                // {
                //     MY_LOG("IMU: %f %f %f", imu_data.acc_x, imu_data.acc_y, imu_data.acc_z);
                // }
                // else
                // {
                //     MY_LOG("IMU data not available");
                // }
                sensorUpdateTime = millis();
            }
            if (millis() - rtcUpdateTime > RTC_UPDATE_INTERVAL)
            {
                tm = readRTC();
                rtcUpdateTime = millis();
            }
            // testFillRainbow();
            // uint16_t randColor = rand() % 64;
            // uint8_t r = (randColor & 0b110000) >> 4;
            // uint8_t g = (randColor & 0b001100) >> 2;
            // uint8_t b = randColor & 0b000011;
            // canvas->fillScreen(randColor);
            // memcpy(canvas->getBuffer(), casio_yellow, 240 * 240);
            if (AppManager::currentApp != nullptr)
            {
                AppManager::currentApp->onDraw(gfx);
            }
            else
            {
                renderCasioWatchFace();
                // flushDisplay(canvas->getBuffer());
                // canvas->setFont(&FreeMonoBold12pt7b);
                // printCenteredText(buf, COLOR_WHITE, 1, 120, 20);
                // canvas->setFont(nullptr);
                // snprintf(buf, sizeof(buf), "R:%d G:%d B:%d", r, g, b);
                // printCenteredText(buf, COLOR_BLACK, 2, 120, 40);
                printCenteredText(power_buf, COLOR_BLACK, 1, 120, 120);
                // memcpy(gfx->getFramebuffer(), music_player, 240 * 240);
                flushDisplay(gfx->getFramebuffer());
                // delay(1000);
            }
            ble_server_loop();
        }
    }
}
