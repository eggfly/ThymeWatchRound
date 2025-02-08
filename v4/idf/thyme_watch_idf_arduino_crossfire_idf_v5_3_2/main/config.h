#pragma once

#define ULP_I2C_SDA (5)
#define ULP_I2C_SCL (4)

#define _2ND_I2C_SDA (13)
#define _2ND_I2C_SCL (12)

#define I2S_DOUT    (2)
#define I2S_BCLK    (1)
#define I2S_LRC     (3)

#define I2C_FREQUENCY 400000 // 100kHz Standard Mode

#define SENSOR_UPDATE_INTERVAL 1000

#define IMU_UPDATE_INTERVAL 20

#define RTC_UPDATE_INTERVAL 200

#define RGB565_ORANGE 0xFAA0

#define REBOOT_PIN GPIO_NUM_0
#define WAKEUP_PIN (0)
#define BACK_PIN (0)
#define BUZZER_PIN (8)

#define ENABLE_POWER_AND_SLEEP_BUZZER   (0)

#define SLEEP_DURATION 10 // 深度睡眠时间（秒）

#define IDLE_DEEP_SLEEP_TIME_SECONDS (120)
#define IDLE_DEEP_SLEEP_TIME_MILLIS (IDLE_DEEP_SLEEP_TIME_SECONDS * 1000)

#define ENABLE_WATCH_BLE_SERVER (1)
