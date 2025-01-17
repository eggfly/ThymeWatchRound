
#include <Arduino.h>
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "SD.h"

// START OF naveitay/PCA6408A@^1.0.0
#include <I2C.hpp> // Assuming I2C library is available in your project
#include <PCA6408A.hpp>
// END OF naveitay/PCA6408A

#include "wifi_radio.h"

#define ULP_I2C_SDA (5)
#define ULP_I2C_SCL (4)
#define I2C_FREQUENCY 400000  // 100kHz Standard Mode
#define PCA6408A_ADDRESS 0x20 // Default address of PCA6408A

#define SENSOR_POWER_PIN (9)
#define ENABLE_NEW_IO_EXPANDER_BACKLIGHT (1)

PCA6408A pca6408a(PCA6408A_ADDRESS);

void initPCA6408A()
{
  // pca6408a.Initialization();
  Serial.println("PCA6408A initialized!");
  pca6408a.setDirection(PCA6408A_IO7_INPUT |
                        PCA6408A_IO6_OUTPUT |
                        PCA6408A_IO5_INPUT |
                        PCA6408A_IO4_OUTPUT |
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
  // Set PCM5102 power on, LOW is on
  pca6408a.setDigital(PCA6408A_IO4, LOW);
  Serial.println("PCM5102 powered on!");
}

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
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  // Enable backlight and audio power
  initPCA6408A();
  Serial.println("Starting Serial");
  radio_setup();
}

void loop()
{
  radio_loop();
  // Serial.println("Hello, World! loop()...");
}
