#include <Arduino.h>

#include "esp_check.h"
#include "driver/dedic_gpio.h"
#include "driver/gpio.h"
#include "soc/gpio_struct.h"

#include "hal_ls012b7dd06.h"

// State variables for toggling
bool hckState = LOW;
bool vckState = LOW;

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

// 实测和 define 宏的刷屏速度一样，所以改成 always inline 函数
static inline __attribute__((always_inline)) void directWriteLow(uint32_t pin)
{
  if (pin < 32)
  {
    GPIO.out_w1tc = ((uint32_t)1 << pin);
  }
  else if (pin <= 48)
  {
    GPIO.out1_w1tc.val = ((uint32_t)1 << (pin - 32));
  }
}

static inline __attribute__((always_inline)) void directWriteHigh(uint32_t pin)
{
  if (pin < 32)
  {
    GPIO.out_w1ts = ((uint32_t)1 << pin);
  }
  else if (pin <= 48)
  {
    GPIO.out1_w1ts.val = ((uint32_t)1 << (pin - 32));
  }
}

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

void flushDisplay(uint8_t *buffer)
{
  digitalWrite(INTB_PIN, HIGH);
  auto counter = portGET_RUN_TIME_COUNTER_VALUE();

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
#ifdef DEBUG_PRINT_SCREEN
  Serial.printf("%ld %ld %ld %ld\n", cost0, cost3, cost4, total_cost);
#endif
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

void initScreenOutputPins()
{
  // pinMode(LED_PIN, OUTPUT);
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
}

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

void initVcomLedc()
{
  ledcAttach(VCOM_PIN, FREQ_VCOM, ledc_resolution);
  ledcWrite(VCOM_PIN, 1 << (ledc_resolution - 1)); // 设置 PWM 占空比 1/2
}
