# PCA6408A Library

The **PCA6408A** library provides an easy-to-use interface for controlling the **PCA6408A** I/O expander chip using the I2C protocol. This library allows you to configure and control up to 8 GPIO pins over I2C, making it ideal for adding additional I/O capabilities to your microcontroller projects.

## Features
- Simple interface for initializing and configuring the PCA6408A I/O expander.
- Set individual I/O pins as input or output.
- Write digital values to output pins.
- Clear all outputs at once.
- Works with standard I2C communication using custom SDA and SCL pins.

## Installation
1. Clone or download this repository.
3. Include the library in your sketch:
   ```cpp
   #include <PCA6408A.hpp>

## Dependencies
This library uses the I2C communication protocol. Make sure you have the I2C library installed or included in your project to use this library.

## Example Usage
Here’s a basic example of how to use the PCA6408A library to control the I/O expander:

```cpp
#include <I2C.hpp> // Assuming I2C library is available
#include <PCA6408A.hpp>

// Define the I2C pins and address
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQUENCY 100000  // 100kHz Standard Mode
#define PCA6408A_ADDRESS 0x20  // Default address of PCA6408A

PCA6408A pca6408a(PCA6408A_ADDRESS);

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C
  if (!i2cBegin(SDA_PIN, SCL_PIN, I2C_FREQUENCY)) {
    Serial.println("I2C initialization failed!");
    while (1);
  }

  // Initialize PCA6408A
  pca6408a.Initialization();
  Serial.println("PCA6408A initialized!");

  // Set IO0 to HIGH
  pca6408a.setDigital(PCA6408A_IO0, HIGH);

  // Set all outputs to LOW
  pca6408a.setAllClear();
  Serial.println("All outputs cleared.");
}

void loop() {
  // Add further control of the IO pins here
}

```
## Datasheet
For detailed technical information about the PCA6408A I/O expander, refer to the included datasheet:
- Location: datasheet/IO Expander PCA6408A.pdf

## API Reference
PCA6408A Class
Constructor
```cpp
PCA6408A(uint8_t addr = PCA6408A_SLAVE_ADDRESS);
```
- addr: The I2C address of the PCA6408A. Defaults to 0x20.

## Methods
- void Initialization(): Initializes the PCA6408A device and configures all pins as output.
- void setDigital(uint8_t port, uint8_t output): Sets the state of a specific I/O pin (HIGH or LOW).
    - port: The I/O pin (e.g., PCA6408A_IO0).
    - output: HIGH or LOW.
- void setAllClear(): Clears all I/O pins, setting them to LOW.
- void setGPIO(uint8_t output): Directly sets the output register with the specified 8-bit value.
