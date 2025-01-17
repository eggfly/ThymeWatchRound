#include "PCA6408A.hpp"

uint8_t current_output_state = 0b00000000; // Initialize to all LOW

PCA6408A::PCA6408A(uint8_t addr)
{
    _i2caddr = addr;
    // Wire.begin();
}

/**
 @brief Configure Device
*/
void PCA6408A::Initialization()
{
    uint8_t conf;
    conf = PCA6408A_IO0_OUTPUT;
    conf |= PCA6408A_IO1_OUTPUT;
    conf |= PCA6408A_IO2_OUTPUT;
    conf |= PCA6408A_IO3_OUTPUT;
    conf |= PCA6408A_IO4_OUTPUT;
    conf |= PCA6408A_IO5_OUTPUT;
    conf |= PCA6408A_IO6_OUTPUT;
    conf |= PCA6408A_IO7_OUTPUT;

    // writeI2c
    i2cWriteByte(_i2caddr, PCA6408A_CONFIGURATION_REG, conf);

    // Initialize all outputs to LOW
    setAllClear();
}

/**
 @brief set Port to Digital
 @param [in] port Port
 @param [in] output output
*/
void PCA6408A::setDigital(uint8_t port, uint8_t output)
{
    if (output == HIGH)
    {
        current_output_state |= port;
    }
    else if (output == LOW)
    {
        current_output_state &= ~port;
    }
    i2cWriteByte(_i2caddr, PCA6408A_OUTPUT_REG, current_output_state);
}

/**
 @brief All Port to LOW
*/
void PCA6408A::setAllClear()
{
    current_output_state = 0b00000000; // Set all outputs to LOW
    i2cWriteByte(_i2caddr, PCA6408A_OUTPUT_REG, current_output_state);
}

bool PCA6408A::readInput(uint8_t *inputs)
{
    return i2cReadByte(_i2caddr, PCA6408A_INPUT_REG, inputs);
}

/**
 @brief set GPIO ports direction
 @param [in] output output
*/
void PCA6408A::setDirection(uint8_t directionConfigBits)
{
    // writeI2c
    i2cWriteByte(_i2caddr, PCA6408A_CONFIGURATION_REG, directionConfigBits);
}
