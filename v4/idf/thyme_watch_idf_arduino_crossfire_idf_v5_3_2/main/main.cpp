#include <stdio.h>
#include <Arduino.h>
#include <Wire.h>

#include "tusb_msc.h"
#include "esp_task_wdt.h"

// #include <Adafruit_GFX.h>
#include <Arduino_GFX_Library.h>
#include "Arduino_Canvas_6bit.h"

#include "util/memory.h"
#include "util/ui.h"
#include "util/system.h"

#include "hal/hal_pca6408a.h"
#include "hal/hal_ls012b7dd06.h"
#include "hal/hal_digital_crown.h"
#include "hal/hal_pressure_sensor.h"
#include "hal/hal_va_meter.h"
#include "hal/hal_imu.h"
#include "hal/hal_rtc.h"
#include "hal/hal_ble_server.h"

#include "config.h"
#include "common.h"
#include "resources/bloxx.h"
#include "resources/jiaming2.h"
#include "resources/jiaming3.h"
#include "resources/music_player.h"

#include "apps/apps.h"
#include "framework/app_manager.h"

// #include "Fonts/FreeMono12pt7b.h"
// #include "Fonts/FreeMonoBold9pt7b.h"
// #include "Fonts/FreeMonoBold12pt7b.h"
// #include "Fonts/FreeMonoBold18pt7b.h"

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
        AppManager::gfx->fillCircle(120, 120, 120 - i * 7, _16_colors[i]);
        // canvas->fillCircle(120, 120, 120 - i * 7, _16_colors[i]);
    }
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

void initArduinoGFX()
{
    AppManager::gfx = new Arduino_Canvas_6bit(240, 240, nullptr);
    // I (409) Memory: Total heap size: 140572 bytes, Max alloc size: 57344 bytes
    // I (409) Memory: Free memory: 119252 bytes, Total memory: 140572 bytes
    // I (409) Memory: No PSRAM found
    // I (410) Thyme: Failed to initialize GFX, no memory?
    print_memory_info();
    AppManager::gfx->setCustomFrameBuffer(AppManager::frame_buffer);
    // bool result = gfx->begin();
    // if (!result)
    // {
    //     MY_LOG("Failed to initialize GFX, no memory?");
    //     while (1)
    //         ;
    // }
    AppManager::gfx->fillScreen(COLOR_WHITE);
    AppManager::gfx->setUTF8Print(true); // enable UTF8 support for the Arduino print() function
    AppManager::gfx->setFont(u8g2_font_unifont_t_chinese);
}

bool gPrevTouched = false;

void mainSetup()
{
    MY_LOG("Now app_main() called, time is %ld", millis());
    AppManager::navigateToApp<ThymeWatchFace>();
    pinMode(BACK_PIN, INPUT);
    initArduino();
    MY_LOG("initArduino() called.");
    // USB CDC doesn't need a baud rate
    Serial.begin();
    Serial.println("Hello esp-idf arduino component!");
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
            memset(AppManager::frame_buffer, _16_colors[i], sizeof(AppManager::frame_buffer));
            flushDisplay(AppManager::frame_buffer);
        }
    }
    // Init acce and gyro meters.
    init_ble_server();
    init_imu();
    // Init va meter
    init_va_meter();
    // Init fuel gauge
    init_fuel_gauge();
    // Init digital crown
    init_pat9125();
    init_pressure_sensor();
    init_sdmmc();

    // Scanner("===  Primary I2C  === ", Wire);
    // Scanner("=== Secondary I2C === ", Wire1);

    testFillRainbow();
    flushDisplay(AppManager::getBuffer());
    // printCenteredText(gfx, "Hello World!", COLOR_BLACK, 3, 120, 120);
    // printCenteredText(gfx, "UP MassStorage ->", COLOR_BLACK, 2, 120, 60);
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
    // uint8_t inputs;
    // if (pca6408a.readInput(&inputs))
    // {
    //     MY_LOG("Inputs: %d", inputs);
    //     bool pin0 = !(inputs & 0b001);
    //     bool pin1 = !(inputs & 0b010);
    //     bool pin2 = !(inputs & 0b100);
    //     if (pin0 || pin1 || pin2)
    //     {
    //         MY_LOG("Enter USB MSC");
    //         enter_usb_msc();
    //         wait_for_key_forever();
    //         /* Simply restart make usb not vailable, dont know why */
    //         // pmu.powerOff();
    //         return;
    //     }
    // }
}

void mainLoop()
{
    if (Serial.available() > 0)
    {
        // 一旦缓冲区内有可读数据，则读取1字节
        int incomingByte = Serial.read();
        // 以十进制打印读取到的字节
        MY_LOG("Received: %d", incomingByte);
    }
    if (AppManager::currentApp != nullptr)
    {
        if (AppManager::currentApp->needUpdateDigitalCrown())
        {
            long scroll_pos = read_pat9125();
            MY_LOG("Scroll pos: %ld", scroll_pos);
            AppManager::checkNotifyDigitalCrown(scroll_pos);
        }
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
        }
    }
    else
    {
        MY_LOG("Failed to read inputs");
    }
    bool backPressed = digitalRead(BACK_PIN) == LOW;
    AppManager::checkNotifyBackButton(backPressed);

    // testFillRainbow();
    // uint16_t randColor = rand() % 64;
    // uint8_t r = (randColor & 0b110000) >> 4;
    // uint8_t g = (randColor & 0b001100) >> 2;
    // uint8_t b = randColor & 0b000011;
    // canvas->fillScreen(randColor);
    // memcpy(canvas->getBuffer(), casio_yellow, 240 * 240);
    if (AppManager::currentApp != nullptr)
    {
        AppManager::currentApp->onDraw(AppManager::gfx);
    }
    // TODO
    ble_server_loop();
}

extern "C"
{
    void app_main(void)
    {
        mainSetup();
        while (true)
        {
            mainLoop();
        }
    }
}
