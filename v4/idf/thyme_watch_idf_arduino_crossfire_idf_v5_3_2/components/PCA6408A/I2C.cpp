#include "I2C.hpp"

bool i2cBegin(uint8_t sda, uint8_t scl, uint32_t frequency)
{
    // Start I2C with given SDA and SCL pins
    if (!Wire.begin(sda, scl))
    {
        Serial.printf("I2C Wire.begin() Failed: SDA: %d, SCL: %d\n", sda, scl);
        return false;
    }

    // Attempt to set I2C clock frequency
    if (!Wire.setClock(frequency))
    {
        Serial.printf("I2C Wire.setClock() Failed: Frequency: %lu Hz\n", frequency);
        return false;
    }

    // If execution reaches here, it means Wire.begin() was successful and Wire.setClock() was either successful or failed silently.
    Serial.printf("I2C Init Successful: SDA: %d, SCL: %d, Frequency: %lu Hz\n", sda, scl, frequency);
    return true;
}

bool i2cReadByte(uint8_t address, uint8_t subAddress, uint8_t *data)
{
    Wire.beginTransmission(address);
    Wire.write(subAddress);
    Wire.endTransmission();
    if (Wire.requestFrom(address, (uint8_t)1) != 1)
    {
        return false;
    }
    *data = Wire.read();
    return true;
}

void i2cWriteByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
    Wire.beginTransmission(address);
    Wire.write(subAddress);
    Wire.write(data);
    Wire.endTransmission();
}

void i2cReadBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t *dest)
{
    Wire.beginTransmission(address);
    Wire.write(subAddress);
    Wire.endTransmission();
    Wire.requestFrom(address, count);
    for (uint8_t i = 0; Wire.available(); i++)
    {
        dest[i] = Wire.read();
    }
}

void i2cWriteBytes(uint8_t address, uint8_t subAddress, uint8_t count, const uint8_t *data)
{
    Wire.beginTransmission(address);
    Wire.write(subAddress);
    for (uint8_t i = 0; i < count; i++)
    {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}
