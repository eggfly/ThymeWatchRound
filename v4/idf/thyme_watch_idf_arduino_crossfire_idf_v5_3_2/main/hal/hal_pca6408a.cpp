#include <Arduino.h>
#include "I2C.hpp"
#include "PCA6408A.hpp"

#define ENABLE_NEW_IO_EXPANDER_BACKLIGHT (1)
#define PCA6408A_ADDRESS 0x20 // Default address of PCA6408A

PCA6408A pca6408a(PCA6408A_ADDRESS);

void initPCA6408A()
{
  // pca6408a.Initialization();
  Serial.println("PCA6408A initialized!");
  pca6408a.setDirection(PCA6408A_IO7_INPUT |
                        PCA6408A_IO6_OUTPUT | /* PCA6408A_IO6 is TP POWER */
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

