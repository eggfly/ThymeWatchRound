#include <Arduino.h>
#include "color_pad.h"
#include "mario.h"
#include "rainbow_frame_1.h"
#include "anim_data_12_frames.h"
#include "anim_data_7_frames.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "esp_check.h"
#include "driver/dedic_gpio.h"
#include "driver/gpio.h"

#include "ParallelColorMemLCD.h"
#include "elk.h"
#include "duktape.h"

// START OF naveitay/PCA6408A@^1.0.0
#include <I2C.hpp> // Assuming I2C library is available in your project
#include <PCA6408A.hpp>
// END OF naveitay/PCA6408A

#include <RTC_RX8025T.h>
#include <TimeLib.h> //https://github.com/PaulStoffregen/Time
#include <RTClib.h>

#define ULP_I2C_SDA (5)
#define ULP_I2C_SCL (4)
#define I2C_FREQUENCY 400000  // 100kHz Standard Mode
#define PCA6408A_ADDRESS 0x20 // Default address of PCA6408A

#define _2ND_I2C_SDA (13)
#define _2ND_I2C_SCL (12)

// Extended ICs Power Control
#define SENSOR_POWER_PIN (9)

#define BUZZER_PIN (8)
#define BUZZER_VOLUME_LEVEL (4)

PCA6408A pca6408a(PCA6408A_ADDRESS);

#define ENABLE_DEEPSLEEP (1)
#define USE_FAST_DIRECT_WRITE (1)

#define NEW_LCD_IO_CONNECTIONS (1)

#define USE_2ND_CORE_PWM (1)

#define USE_CONST_VCOM_NOT_PWM (0)

#define ENABLE_NEW_IO_EXPANDER_BACKLIGHT (1)

#define ENABLE_OLD_PWM_BACKLIGHT (0)

#define ENABLE_BOOT_BUZZER (1)

// USE: 19801 us per frame, NOT USE: 118875 us per frame
#define USE_S3_DEDICATED_GPIO (1)

#define FREQ_VCOM 60
#define BL_FREQ 120

const int ledChannel = 0;  // LEDC 通道
const int resolution = 12; // 分辨率（12 位）, 8位好像只有ESP32可以用

const int BACKLIGHT_LEDC_CHANNEL = 1;
const int BUZZER_LEDC_CHANNEL = 2;

// BL
#define BACKLIGHT_PIN 16

// BOOT PIN
#define WAKEUP_PIN (0)

#if !defined(NEW_LCD_IO_CONNECTIONS)
// Pin definitions
// GSP
#define GSP_PIN 1
// GCK
#define GCK_PIN 2
// GEN
#define GEN_PIN 3
// INTB
#define INTB_PIN 4
// VCOM same as FRP = VB
#define FRP_PIN 5
// VA = XFRP
#define XFRP_PIN 6

#define BSP_PIN 7
// BCK
#define BCK_PIN 8

#define R0_PIN 9
#define R1_PIN 10
#define G0_PIN 11
#define G1_PIN 12
#define B0_PIN 13
#define B1_PIN 14

// VCOM
#define VCOM_PIN 15

#else

// Pin definitions
#define GSP_PIN 11

#define GCK_PIN 41

#define GEN_PIN 46

#define INTB_PIN 40
// VCOM same as FRP = VB
// #define FRP_PIN 10
// VA = XFRP
// #define XFRP_PIN 9

// BSP only appears in horizontal signal
#define BSP_PIN 45
// BCK only appears in horizontal signal
#define BCK_PIN 39

#define R0_PIN 44
#define R1_PIN 38

#define G0_PIN 43
#define G1_PIN 47

#define B0_PIN 42
#define B1_PIN 48

// VCOM
#define VCOM_PIN 10

#endif

// NO USE
#define LED_PIN 21

#define SLEEP_DURATION 10 // 深度睡眠时间（秒）

#define COLOR_BLACK 0b00000000      // 黑色
#define COLOR_WHITE 0b00111111      // 白色
#define COLOR_RED 0b00110000        // 红色
#define COLOR_GREEN 0b00001100      // 绿色
#define COLOR_BLUE 0b00000011       // 蓝色
#define COLOR_CYAN 0b00001111       // 青色
#define COLOR_MAGENTA 0b00110011    // 品红
#define COLOR_YELLOW 0b00111100     // 黄色
#define COLOR_GRAY 0b00110000       // 灰色
#define COLOR_DARK_RED 0b00100000   // 深红
#define COLOR_DARK_GREEN 0b00001000 // 深绿
#define COLOR_DARK_BLUE 0b00000010  // 深蓝
#define COLOR_ORANGE 0b00110100     // 橙色
#define COLOR_PURPLE 0b00100100     // 紫色
#define COLOR_BROWN 0b00100010      // 棕色
#define COLOR_LIGHT_GRAY 0b00111000 // 浅灰色

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
    COLOR_LIGHT_GRAY};

// ESP32 GPIO:
// 1: TX0 PIN
// 3: RX0 PIN
// 6 - 11 CANNOT USE, connected to the integrated SPI flash
// 34 - 39 INPUT ONLY
void resetDisplay()
{
  digitalWrite(INTB_PIN, HIGH);
  delayMicroseconds(50);
  digitalWrite(INTB_PIN, LOW);
  delayMicroseconds(20);
  digitalWrite(INTB_PIN, HIGH);
}

void initDisplay()
{
  digitalWrite(GCK_PIN, LOW);
  digitalWrite(GSP_PIN, LOW);
  digitalWrite(BSP_PIN, LOW);
  digitalWrite(BCK_PIN, LOW);
  digitalWrite(GEN_PIN, LOW);
}

void initScreenPWMPins()
{
  Serial.println("initScreenPWMPins");
  pinMode(VCOM_PIN, OUTPUT);
  // pinMode(FRP_PIN, OUTPUT);
  // pinMode(XFRP_PIN, OUTPUT);

  digitalWrite(VCOM_PIN, LOW);
  // digitalWrite(FRP_PIN, LOW);
  // digitalWrite(XFRP_PIN, HIGH);
}

// put function definitions here:
void anotherTask(void *parameter)
{
  initScreenPWMPins();
  size_t loopCount = 0;
  while (true)
  {
    digitalWrite(VCOM_PIN, HIGH);
    // digitalWrite(FRP_PIN, HIGH);
    // digitalWrite(XFRP_PIN, LOW);
    delayMicroseconds(1000 * 1000 / FREQ_VCOM / 2);
    digitalWrite(VCOM_PIN, LOW);
    // digitalWrite(FRP_PIN, LOW);
    // digitalWrite(XFRP_PIN, HIGH);
    delayMicroseconds(1000 * 1000 / FREQ_VCOM / 2);
    // while (1)
    // {
    //   delay(1000);
    //   // Serial.printf("anotherTask %d\n", loopCount);
    // }
    loopCount++;
  }
}

// State variables for toggling
bool hckState = LOW;
bool vckState = LOW;

// static inline __attribute__((always_inline)) void directWriteLow(IO_REG_TYPE pin)
// {
//   if (pin < 32)
//     GPIO.out_w1tc = ((uint32_t)1 << pin);
//   else if (pin < 34)
//     GPIO.out1_w1tc.val = ((uint32_t)1 << (pin - 32));
// }

// static inline __attribute__((always_inline)) void directWriteHigh(IO_REG_TYPE pin)
// {
//   if (pin < 32)
//     GPIO.out_w1ts = ((uint32_t)1 << pin);
//   else if (pin < 34)
//     GPIO.out1_w1ts.val = ((uint32_t)1 << (pin - 32));
// }

#define MY_DIGITAL_WRITE(pin, value) \
  if (USE_FAST_DIRECT_WRITE)         \
  {                                  \
    if (value)                       \
    {                                \
      directWriteHigh(pin);          \
    }                                \
    else                             \
    {                                \
      directWriteLow(pin);           \
    }                                \
  }                                  \
  else                               \
  {                                  \
    digitalWrite(pin, value);        \
  }

#define directWriteLow(pin)                               \
  do                                                      \
  {                                                       \
    if ((pin) < 32)                                       \
      GPIO.out_w1tc = ((uint32_t)1 << (pin));             \
    else                                                  \
      GPIO.out1_w1tc.val = ((uint32_t)1 << ((pin) - 32)); \
  } while (0)

#define directWriteHigh(pin)                              \
  do                                                      \
  {                                                       \
    if ((pin) < 32)                                       \
      GPIO.out_w1ts = ((uint32_t)1 << (pin));             \
    else                                                  \
      GPIO.out1_w1ts.val = ((uint32_t)1 << ((pin) - 32)); \
  } while (0)

dedic_gpio_bundle_handle_t bundleA = NULL;

void dedic_gpio_new()
{
  const int bundleA_gpios[] = {
      B1_PIN,
      B0_PIN,
      G1_PIN,
      G0_PIN,
      R1_PIN,
      R0_PIN,
  };
  for (size_t i = 0; i < sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]); i++)
  {
    pinMode(bundleA_gpios[i], OUTPUT);
  }
  gpio_config_t io_conf = {
      .mode = GPIO_MODE_OUTPUT,
  };
  for (int i = 0; i < sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]); i++)
  {
    io_conf.pin_bit_mask = 1ULL << bundleA_gpios[i];
    gpio_config(&io_conf);
  }
  // 创建 bundleA，仅输出
  dedic_gpio_bundle_config_t bundleA_config = {
      .gpio_array = bundleA_gpios,
      .array_size = sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]),
      .flags = {
          .out_en = 1,
      },
  };
  ESP_ERROR_CHECK(dedic_gpio_new_bundle(&bundleA_config, &bundleA));
}

// static inline __attribute__((always_inline)) void soft_fast_gpio_write(uint8_t _8bit)
// {
//   // extern dedic_gpio_bundle_handle_t bundleA;
//   dedic_gpio_bundle_write(bundleA, 0b00111111, _8bit);
// }

#define soft_fast_gpio_write(_8bit) dedic_gpio_bundle_write(bundleA, 0b00111111, _8bit)

auto cost1 = micros();
auto cost2 = micros();
auto cost0 = portGET_RUN_TIME_COUNTER_VALUE();
auto cost3 = portGET_RUN_TIME_COUNTER_VALUE();
auto cost4 = portGET_RUN_TIME_COUNTER_VALUE();
auto cost5 = portGET_RUN_TIME_COUNTER_VALUE();

// Function to flush the framebuffer to the display
// cost 300+ms
void flushDisplay()
{
  digitalWrite(INTB_PIN, HIGH);
  auto counter = portGET_RUN_TIME_COUNTER_VALUE();

  uint8_t *buffer = canvas->getBuffer();
  digitalWrite(GSP_PIN, HIGH);
  // delayMicroseconds(20000);
  vckState = !vckState;
  digitalWrite(GCK_PIN, vckState);

  for (int i = 1; i <= 487; i++)
  {
    if (i == 1)
    {
      MY_DIGITAL_WRITE(GSP_PIN, LOW);
      // delayMicroseconds(21000);
    }

    if (i >= 2 && i <= 481)
    {
      MY_DIGITAL_WRITE(BSP_PIN, HIGH);
      hckState = !hckState; // Toggle HCK state
      // digitalWrite(BCK_PIN, hckState);
      if (hckState)
      {
        MY_DIGITAL_WRITE(BCK_PIN, HIGH);
      }
      else
      {
        MY_DIGITAL_WRITE(BCK_PIN, LOW);
      }

      // delayMicroseconds(1000);
      // 121 -> 123 了
      // 夏普需要123，并且一定是123
      // JDI 是 121
      cost0 = portGET_RUN_TIME_COUNTER_VALUE();
      cost3 = portGET_RUN_TIME_COUNTER_VALUE() - cost0; // 19 -> 76ns
      for (int j = 1; j <= 123; j++)
      {
        if (j == 1)
        {
          MY_DIGITAL_WRITE(BSP_PIN, LOW);
        }
        // 夏普屏在最后 ENB，JDI 在 20个 CLOCK的时候做 ENB
        if (j == 122)
        {
          MY_DIGITAL_WRITE(GEN_PIN, HIGH);
        }
        else if (j == 123)
        {
          MY_DIGITAL_WRITE(GEN_PIN, LOW);
        }

        if (j >= 2 && j <= 121)
        {
          int y = (i - 2) / 2;
          int x = j - 2;
          int pos = (x * 2) + 240 * y;

          // Send pixel data
          uint8_t pixel = buffer[pos];
          // 这里 JDI 和 夏普屏都需要有 LPB/SPB 的区别，所以必须有下面的 if/else
          uint8_t _6bitColor = 0;
          if (i % 2 == 0)
          {
            // LPB
            if (USE_S3_DEDICATED_GPIO)
            {
              // _6bitColor |= (pixel & 0b100000) ? 0b100000 : 0;
              // _6bitColor |= (pixel & 0b001000) ? 0b001000 : 0;
              // _6bitColor |= (pixel & 0b000010) ? 0b000010 : 0;
              // simplify to below
              _6bitColor |= (pixel & 0b101010);
              pixel = buffer[pos + 1];
              // _6bitColor |= (pixel & 0b100000) ? 0b010000 : 0;
              // _6bitColor |= (pixel & 0b001000) ? 0b000100 : 0;
              // _6bitColor |= (pixel & 0b000010) ? 0b000001 : 0;
              // simplify to below
              _6bitColor |= (pixel & 0b101010) >> 1;
              // cpu tick is 4ns
              // cost0 = portGET_RUN_TIME_COUNTER_VALUE();
              // cost3 = portGET_RUN_TIME_COUNTER_VALUE() - cost0; // 20 -> 80ns
              soft_fast_gpio_write(_6bitColor);
              // cost4 = portGET_RUN_TIME_COUNTER_VALUE() - cost0; // 64 -> 64*4 = 256ns
            }
            else
            {
              digitalWrite(R0_PIN, (pixel & 0b100000) ? HIGH : LOW);
              digitalWrite(G0_PIN, (pixel & 0b001000) ? HIGH : LOW);
              digitalWrite(B0_PIN, (pixel & 0b000010) ? HIGH : LOW);
              pixel = buffer[pos + 1];
              digitalWrite(R1_PIN, (pixel & 0b100000) ? HIGH : LOW);
              digitalWrite(G1_PIN, (pixel & 0b001000) ? HIGH : LOW);
              digitalWrite(B1_PIN, (pixel & 0b000010) ? HIGH : LOW);
            }
          }
          else
          {
            // SPB
            if (USE_S3_DEDICATED_GPIO)
            {
              // _6bitColor |= (pixel & 0b010000) ? 0b100000 : 0;
              // _6bitColor |= (pixel & 0b000100) ? 0b001000 : 0;
              // _6bitColor |= (pixel & 0b000001) ? 0b000010 : 0;
              // simplify to below
              _6bitColor |= (pixel & 0b010101) << 1;
              pixel = buffer[pos + 1];
              // _6bitColor |= (pixel & 0b010000) ? 0b010000 : 0;
              // _6bitColor |= (pixel & 0b000100) ? 0b000100 : 0;
              // _6bitColor |= (pixel & 0b000001) ? 0b000001 : 0;
              // simplify to below
              _6bitColor |= (pixel & 0b010101);
              soft_fast_gpio_write(_6bitColor);
            }
            else
            {
              digitalWrite(R0_PIN, (pixel & 0b010000) ? HIGH : LOW);
              digitalWrite(G0_PIN, (pixel & 0b000100) ? HIGH : LOW);
              digitalWrite(B0_PIN, (pixel & 0b000001) ? HIGH : LOW);
              pixel = buffer[pos + 1];
              digitalWrite(R1_PIN, (pixel & 0b010000) ? HIGH : LOW);
              digitalWrite(G1_PIN, (pixel & 0b000100) ? HIGH : LOW);
              digitalWrite(B1_PIN, (pixel & 0b000001) ? HIGH : LOW);
            }
          }
        }

        // Toggle HCK state
        // TODO
        // delayMicroseconds(500);
        hckState = !hckState;
        MY_DIGITAL_WRITE(BCK_PIN, hckState ? HIGH : LOW);
      }
      cost4 = portGET_RUN_TIME_COUNTER_VALUE() - cost0; // 9753 ticks -> 9753*4 = 39012ns: 39us
    }
    else
    {
      // delayMicroseconds(1000);
    }

    // delayMicroseconds(500);
    // Toggle VCK state
    vckState = !vckState;
    MY_DIGITAL_WRITE(GCK_PIN, vckState ? HIGH : LOW);
  }
  // 19800 us / 4709264 ticks * 1000 = 4.2 nano seconds per tick
  auto total_cost = portGET_RUN_TIME_COUNTER_VALUE() - counter; // 4709264 ticks, 19800 us

  digitalWrite(INTB_PIN, LOW);
  Serial.printf("%ld %ld %ld %ld\n", cost0, cost3, cost4, total_cost);
}

uint8_t colors[] = {
    0b010000,
    0b100000,
    0b110000,
    0b000100,
    0b001000,
    0b001100,
    0b000001,
    0b000010,
    0b000011,
};
size_t currColorIndex = 0;
size_t maxColorIndex = sizeof(colors) / sizeof(colors[0]);

void testFillCircle()
{
  auto x = random(0, LCD_DISP_WIDTH);
  auto y = random(0, LCD_DISP_HEIGHT);
  uint8_t color = random(0, 64);
  canvas->fillCircle(x, y, 32, color);
}

size_t current16ColorsIndex = 0;

void testFillRainbow()
{
  // canvas->fillCircle
  for (int i = 0; i < 16; i++)
  {
    canvas->fillCircle(120, 120, 120 - i * 7, _16_colors[i]);
  }
  // canvas->fillScreen(_16_colors[current16ColorsIndex]);
  current16ColorsIndex++;
  current16ColorsIndex = current16ColorsIndex % 16;
  Serial.printf("current16ColorsIndex: %d\n", current16ColorsIndex);
}

void drawInitialContent()
{
  // Example: Draw "Hello World" and some lines
  // This function should implement the drawing logic similar to the Rust code
  // For simplicity, we will just fill the frame buffer with a color

  // Fill the frame buffer with a color (e.g., white)
  for (int y = 0; y < 240; y++)
  {
    for (int x = 0; x < 240; x++)
    {
      int index = (y * 240 + x); // 1 byte per pixel
      canvas->getBuffer()[index] = colors[currColorIndex];
    }
  }
  currColorIndex++;
  currColorIndex = currColorIndex % maxColorIndex;
  // Send the frame buffer to the display
  // flushDisplay();
}

void drawInitialContent2()
{
  canvas->fillScreen(colors[currColorIndex]);
  currColorIndex++;
  currColorIndex = currColorIndex % maxColorIndex;
  // canvas->drawCircle(120, 120, 16, 0b111111);
  // Send the frame buffer to the display
  // flushDisplay();
  canvas->fillCircle(120, 120, 16, 0b111111);
}

// void initLedc()
// {
//   ledcSetup(ledChannel, FREQ_VCOM, resolution);
//   ledcSetup(1, FREQ_VCOM, resolution);
//   delayMicroseconds(1000 * 1000 / FREQ_VCOM / 2);
//   ledcSetup(2, FREQ_VCOM, resolution);
//   // 将 LEDC 通道连接到 GPIO
//   ledcAttachPin(VCOM_PIN, ledChannel);
//   ledcAttachPin(FRP_PIN, 1);
//   // delayMicroseconds(1000 * 1000 / FREQ_VCOM / 2);
//   ledcAttachPin(XFRP_PIN, 2);

//   ledcWrite(ledChannel, 128); // 设置 PWM 占空比
//   ledcWrite(1, 128);
//   // 高低电平半个周期
//   // delayMicroseconds(1000 * 1000 / FREQ_VCOM / 2);
//   ledcWrite(2, 128);
// }

void setup2()
{
  Serial.begin(115200);
  delay(2000);
}

void loop2()
{
  // gpio_deep_sleep_hold_dis()
  // gpio_hold_en(GPIO_NUM_16);

  auto t = touchRead(1);
  Serial.printf("%d\n", t);
  delay(200);
}

char mem[200];

void test_duktape()
{
  duk_context *ctx = duk_create_heap_default();
  duk_eval_string(ctx, "1+2");
  printf("1+2=%d\n", (int)duk_get_int(ctx, -1));
  duk_destroy_heap(ctx);
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
void Scanner(const char *headerText, TwoWire &Wire)
{
  Serial.print(headerText);
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);       // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0) // Receive 0 = success (ACK response)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX); // PCF8574 7 bit address
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count, DEC); // numbers of devices
  Serial.println(" device(s).");
}

void initPCA6408A()
{
  // pca6408a.Initialization();
  Serial.println("PCA6408A initialized!");
  pca6408a.setDirection(PCA6408A_IO7_INPUT |
                        PCA6408A_IO6_OUTPUT |
                        PCA6408A_IO5_INPUT |
                        PCA6408A_IO4_INPUT |
                        PCA6408A_IO3_OUTPUT |
                        PCA6408A_IO2_INPUT |
                        PCA6408A_IO1_INPUT |
                        PCA6408A_IO0_INPUT);
  if (ENABLE_NEW_IO_EXPANDER_BACKLIGHT)
  {
    // Set IO3 to HIGH
    pca6408a.setDigital(PCA6408A_IO3, HIGH);
  }
  // Set TP 3V3 Open
  pca6408a.setDigital(PCA6408A_IO6, HIGH);
}

void initRTC();

void setup()
{
  Serial.begin(115200);
  Wire.end();
  // Initialize I2C
  if (!i2cBegin(ULP_I2C_SDA, ULP_I2C_SCL, I2C_FREQUENCY))
  {
    Serial.println("I2C initialization failed!");
    while (1)
      ;
  }
  delay(1500);
  Serial.println("beginning of setup()");
  // Secondary I2C must be initialized after the SENSOR Power On
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  if (Wire1.begin(_2ND_I2C_SDA, _2ND_I2C_SCL, I2C_FREQUENCY) == 0)
  {
    Serial.println("I2C1 initialization failed!");
    while (1)
      ;
  }
  initPCA6408A();
  if (ENABLE_BOOT_BUZZER)
  {
    ledcSetup(BUZZER_LEDC_CHANNEL, 4000, resolution);
    ledcAttachPin(BUZZER_PIN, BUZZER_LEDC_CHANNEL);
    ledcWrite(BUZZER_LEDC_CHANNEL, 1 << (resolution - BUZZER_VOLUME_LEVEL) - 1);
    delay(40);
    ledcWrite(BUZZER_LEDC_CHANNEL, 0);
    // ledcDetachPin(BUZZER_PIN);
  }
  // elk: 1727629 -> 1727917 bytes = 288 bytes
  struct js *js = js_create(mem, sizeof(mem)); // Create JS instance
  test_duktape();                              // 1507161 bytes -> 1727917 bytes = 221756 bytes

  Scanner("===  Primary I2C  === ", Wire);
  Scanner("=== Secondary I2C === ", Wire1);

  initRTC();
  if (ENABLE_OLD_PWM_BACKLIGHT)
  {
    // pinMode(BACKLIGHT_PIN, OUTPUT);
    ledcSetup(BACKLIGHT_LEDC_CHANNEL, BL_FREQ, resolution);
    ledcAttachPin(BACKLIGHT_PIN, BACKLIGHT_LEDC_CHANNEL);
    ledcWrite(BACKLIGHT_LEDC_CHANNEL, 1 << (resolution - 1) - 1);
    // digitalWrite(BACKLIGHT_PIN, HIGH);
  }

  canvas = new GFXcanvas8(240, 240);
  // delay(500);
  Serial.println("after new GFXcanvas8 and delay");

  if (USE_CONST_VCOM_NOT_PWM)
  {
    initScreenPWMPins();
  }
  else
  {
    if (USE_2ND_CORE_PWM)
    {
      int coreId = xPortGetCoreID();
      int anotherCoreId = 1 - coreId;
      Serial.printf("Core ID: %d\n", coreId);
      xTaskCreatePinnedToCore(
          anotherTask,    /* Function to implement the task */
          "anotherTask",  /* Name of the task */
          10000,          /* Stack size in words */
          NULL,           /* Task input parameter */
          0,              /* Priority of the task */
          NULL,           /* Task handle. */
          anotherCoreId); /* Core where the task should run */
    }
    else
    {
      // initLedc();
    }
  }

  pinMode(LED_PIN, OUTPUT);
  pinMode(INTB_PIN, OUTPUT);
  pinMode(GSP_PIN, OUTPUT);
  pinMode(GCK_PIN, OUTPUT);
  pinMode(GEN_PIN, OUTPUT);
  pinMode(BSP_PIN, OUTPUT);
  pinMode(BCK_PIN, OUTPUT);
  pinMode(R0_PIN, OUTPUT);
  pinMode(R1_PIN, OUTPUT);
  pinMode(G0_PIN, OUTPUT);
  pinMode(G1_PIN, OUTPUT);
  pinMode(B0_PIN, OUTPUT);
  pinMode(B1_PIN, OUTPUT);

  if (USE_S3_DEDICATED_GPIO)
  {
    dedic_gpio_new();
  }
  // Initialize the display
  resetDisplay();
  Serial.println("after resetDisplay");
  // delay(3000);
  initDisplay();
  Serial.println("after initDisplay");
  // delay(3000);
  // drawInitialContent2();
  // memcpy(canvas->getBuffer(), mario, 240 * 240);
  // memcpy(canvas->getBuffer(), images, 240 * 240);
}

size_t frame_index = 0;

void copyMarioImageBuffer()
{
  memcpy(canvas->getBuffer(), mario, 240 * 240);
}

void printCenteredText(const char *text, uint8_t color, uint8_t size, uint16_t centerX, uint16_t centerY)
{
  canvas->setTextColor(color);
  canvas->setTextSize(size);
  uint16_t width, height;
  int16_t x1, y1;
  canvas->getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
  canvas->setCursor(centerX - width / 2, centerY - height / 2);
  canvas->print(text);
}

bool isBit8Set(uint8_t value)
{
  return (value & (1 << 7)) != 0;
}

void readPCA6408A()
{
  uint8_t inputs;
  bool success = pca6408a.readInput(&inputs);
  bool isTouched = isBit8Set(inputs);
  Serial.printf("PCA6408A IO STATUS: inputs=%d, touched=%d, success=%d\n", inputs, isTouched, success);
}

tmElements_t tm;
const char *dayOfWeekName[] =
    {
        "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

const char *monthName[] =
    {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

void printDateTime(void)
{
  Serial.print("Date: ");
  // Time library macro for convenient Year conversion to calendar format
  Serial.print(tmYearToCalendar(tm.Year));
  Serial.print(" ");
  Serial.print(monthName[tm.Month]);
  Serial.print(" ");
  if (tm.Day < 10)
    Serial.print("0");
  Serial.print(tm.Day);
  Serial.print(" ");
  // tm.Wday stores values from 1 to 7, so we have to subtract 1 to correctly read the names from the array
  Serial.print(dayOfWeekName[tm.Wday - 1]);

  Serial.print("     Time: ");
  if (tm.Hour < 10)
    Serial.print("0");
  Serial.print(tm.Hour);
  Serial.print(":");
  if (tm.Minute < 10)
    Serial.print("0");
  Serial.print(tm.Minute);
  Serial.print(":");
  if (tm.Second < 10)
    Serial.print("0");
  Serial.println(tm.Second);
}

void readRTC()
{
  RTC_RX8025T.read(tm);
  printDateTime();
}

void initRTC()
{
  // RX8025T initialization
  RTC_RX8025T.init();
  readRTC();
  Serial.printf("RTC Year: %d\n", tm.Year);
  // 默认是 2000 年
  if (tm.Year <= 30)
  {
    // NEED adjust time
    // The Time library is a software RTC_RX8025T.
    //"system time" it is referring to the current time according to the software RTC_RX8025T.
    // Set the system time to 10h 23m 30s on 25 Oct 2022
    DateTime dateTime(F(__DATE__), F(__TIME__));
    // setTime(10, 23, 30, 25, 10, 22);
    auto ts = dateTime.unixtime();
    // 这一步不会写到 I2C
    setTime(ts);
    // Set the RTC from the system time
    RTC_RX8025T.set(now());
    Serial.printf("RTC_RX8025T set to %s %s, ts=%ld\n", __DATE__, __TIME__, ts);
  }
  else
  {
    Serial.println("RTC_RX8025T No need to adjust time.");
  }
}

void loop()
{
  Serial.printf("beginning of loop(), frame_index: %d\n", frame_index);
  readPCA6408A();
  readRTC();
  // canvas->fillCircle(120, 120, 16, 0b110000);
  // testFillRainbow();
  // testFillCircle();
  //
  switch (frame_index)
  {
  case 0:
    testFillRainbow();
    printCenteredText("WOKE UP", COLOR_WHITE, 5, LCD_DISP_WIDTH / 2, LCD_DISP_HEIGHT / 2);
    flushDisplay();
    delay(1000);
    break;
  case 1:
    canvas->fillScreen(0b010000);
    break;
  case 2:
    canvas->fillScreen(0b100000);
    break;
  case 3:
    canvas->fillScreen(0b110000);
    break;
  case 4:
    canvas->fillScreen(0b000100);
    break;
  case 5:
    canvas->fillScreen(0b001000);
    break;
  case 6:
    canvas->fillScreen(0b001100);
    break;
  case 7:
    canvas->fillScreen(0b000001);
    break;
  case 8:
    canvas->fillScreen(0b000010);
    break;
  case 9:
    canvas->fillScreen(0b000011);
    break;
  case 10:
    copyMarioImageBuffer();
    break;
  case 11:
    memcpy(canvas->getBuffer(), images, 240 * 240);
    break;
  case 12:
    Serial.println("before case 12 (girls)");
    for (int repeat = 0; repeat < 1; repeat++)
    {
      for (int frame = 0; frame < 7; frame++)
      {
        memcpy(canvas->getBuffer(), anim_data_7[frame], 240 * 240);
        flushDisplay();
        delay(1000);
        readPCA6408A();
      }
    }
    // memcpy(canvas->getBuffer(), rainbow_image_data, 240 * 240);
    break;
  case 13:
    Serial.println("before case 13 (rainbow cat)");
    for (int repeat = 0; repeat < 10; repeat++)
    {
      readPCA6408A();
      auto m = micros();
      for (int frame = 0; frame < 12; frame++)
      {
        memcpy(canvas->getBuffer(), anim_data[frame], 240 * 240);
        flushDisplay();
        // delay(80);
      }
      Serial.printf("12 frames took %d us, %d us per frame\n", micros() - m, (micros() - m) / 12);
    }
    // memcpy(canvas->getBuffer(), rainbow_image_data, 240 * 240);
    break;
  case 14:
    testFillRainbow();
    flushDisplay();
    delay(3000);
    break;
  default:
    break;
  }
  // canvas->fillCircle(120, 120, 3, 0b111111);
  Serial.println("after testFillCircle");
  Serial.println("before flushDisplay");
  auto t = micros();
  uint8_t x = 0; // 1就没了，0更没有
  uint8_t x2 = 239;
  canvas->drawLine(x, 0, x, 240, 0b111111);
  canvas->drawLine(x2, 0, x2, 240, 0b111111);
  flushDisplay();
  frame_index++;
  if (frame_index >= 14)
  {
    if (ENABLE_DEEPSLEEP)
    {
      copyMarioImageBuffer();
      canvas->setTextColor(COLOR_MAGENTA);
      uint16_t width, height;
      int16_t x1, y1;
      const char *text = "SLEEP!!";
      canvas->setTextSize(2);
      canvas->getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
      Serial.printf("measured: width: %d, height: %d\n", width, height);
      canvas->setCursor(LCD_DISP_WIDTH / 2 - width / 2, LCD_DISP_HEIGHT - height - 5);
      canvas->print(text);
      flushDisplay();
      pca6408a.setDigital(PCA6408A_IO3, LOW);
      // digitalWrite(INTB_PIN, LOW);
      // gpio_hold_en((gpio_num_t)INTB_PIN);
      gpio_hold_en((gpio_num_t)VCOM_PIN);
      // hold 上面好像还不够，还是会有问题
      // gpio_deep_sleep_hold_en();
      // esp_deep_sleep(3000000);
      esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_PIN, 0); // 0 表示低电平唤醒
      esp_sleep_enable_timer_wakeup(SLEEP_DURATION * 1000000ULL);

      Serial.println("Going to sleep now");
      // delay(2000);
      // ESP.restart();
      esp_deep_sleep_start();
    }
    frame_index = 0;
  }
  delay(500);
  Serial.printf("flushDisplay took %d us\n", micros() - t);

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  Serial.println("after LED");
}
