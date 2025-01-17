#pragma once
#include <Arduino.h>


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

#define USE_FAST_DIRECT_WRITE (1)

#define USE_CONST_VCOM_NOT_PWM (0)

// USE: 19801 us per frame, NOT USE: 118875 us per frame
#define USE_S3_DEDICATED_GPIO (1)

#define LCD_WIDTH (240)
#define LCD_HEIGHT (240)

#define FREQ_VCOM 60
#define BL_FREQ 120

const int ledc_resolution = 12; // 分辨率（12 位）, 8位好像只有ESP32可以用

// TODO: has wrong color values
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


void flushDisplay(uint8_t *buffer);
void initScreenPWMPins();
void initScreenOutputPins();
void resetDisplay();
void initDisplay();
void initVcomLedc();
