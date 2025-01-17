#pragma once

#define REBOOT_PIN GPIO_NUM_0

#define ULP_I2C_SDA (5)
#define ULP_I2C_SCL (4)

#define _2ND_I2C_SDA (13)
#define _2ND_I2C_SCL (12)

#define I2C_FREQUENCY 400000 // 100kHz Standard Mode

#define SENSOR_UPDATE_INTERVAL 1000

#define RTC_UPDATE_INTERVAL 200

#define RGB565_ORANGE 0xFAA0

#define WAKEUP_PIN (0)

#define SLEEP_DURATION 10 // 深度睡眠时间（秒）
