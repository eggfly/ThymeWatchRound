#include <I2C.hpp> // Assuming I2C library is available in your project
#include <PCA6408A.hpp>

// Define the I2C pins and address
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQUENCY 100000  // 100kHz Standard Mode
#define PCA6408A_ADDRESS 0x20 // Default address of PCA6408A

PCA6408A pca6408a(PCA6408A_ADDRESS);

void setup()
{
    Serial.begin(115200);

    // Initialize I2C
    if (!i2cBegin(SDA_PIN, SCL_PIN, I2C_FREQUENCY))
    {
        Serial.println("I2C initialization failed!");
        while (1)
            ;
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

void loop()
{
    // Add further control of the IO pins here
}
