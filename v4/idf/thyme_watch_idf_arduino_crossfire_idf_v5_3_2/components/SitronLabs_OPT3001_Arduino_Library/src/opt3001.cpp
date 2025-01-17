/* Self header */
#include "opt3001.h"

/* Arduino libraries */
#include <Wire.h>

/**
 * Reads the contents of the given register.
 * @param[in] reg_address The address of the register.
 * @param[out] reg_content A pointer to a variable that will be updated with the contents of the register.
 * @return 0 in case of success, or a negative error code otherwise.
 */
int opt3001::register_read(const enum opt3001_register reg_address, uint16_t *const reg_content) {
    int res;

    /* Ensure library has been configured */
    if (m_i2c_library == NULL) {
        return -EINVAL;
    }

    /* Send register address */
    m_i2c_library->beginTransmission(m_i2c_address);
    m_i2c_library->write(reg_address);
    res = m_i2c_library->endTransmission(false);
    if (res != 0) {
        return -EIO;
    }

    /* Read data */
    m_i2c_library->requestFrom(m_i2c_address, (uint8_t)2, (uint8_t) true);
    res = m_i2c_library->available();
    if (res == 0) {
        return -EIO;
    }
    *reg_content = m_i2c_library->read();
    *reg_content <<= 8;
    *reg_content |= m_i2c_library->read();

    /* Return success */
    return 0;
}

/**
 * Updates the content of the given register.
 * @param[in] reg_address The address of the register.
 * @param[in] reg_content The new content of the register.
 * @return 0 in case of success, or a negative error code otherwise.
 */
int opt3001::register_write(const enum opt3001_register reg_address, const uint16_t reg_content) {
    int res;

    /* Ensure library has been configured */
    if (m_i2c_library == NULL) {
        return -EINVAL;
    }

    /* Send register address and data */
    m_i2c_library->beginTransmission(m_i2c_address);
    m_i2c_library->write(reg_address);
    m_i2c_library->write((uint8_t)(reg_content >> 8));
    m_i2c_library->write((uint8_t)(reg_content >> 0));
    res = m_i2c_library->endTransmission(true);
    if (res != 0) {
        return -EIO;
    }

    /* Return success */
    return 0;
}

/**
 *
 * @param[in] i2c_library
 * @param[in] i2c_address
 * @return 0 in case of success, or a negative error code otherwise.
 */
int opt3001::setup(TwoWire &i2c_library, const uint8_t i2c_address) {

    /* Ensure i2c address is valid */
    if ((i2c_address & 0b11111100) != 0b01000100) {
        return -EINVAL;
    }

    /* Remember i2c library and address */
    m_i2c_address = i2c_address;
    m_i2c_library = &i2c_library;

    /* Return success */
    return 0;
}

/**
 *
 * @return 0 if a valid device was found, or a negative error code otherwise.
 */
int opt3001::detect(void) {
    int res;

    /* Ensure manufacturer id is as expected */
    uint16_t reg_manufacturer_id = 0x0000;
    res = register_read(OPT3001_REGISTER_MANUID, &reg_manufacturer_id);
    if (res < 0) {
        return -EIO;
    }
    if (reg_manufacturer_id != 0x5449) {
        return -1;
    }

    /* Ensure device id is as expected */
    uint16_t reg_device_id = 0x0000;
    res = register_read(OPT3001_REGISTER_DEVIID, &reg_device_id);
    if (res < 0) {
        return -EIO;
    }
    if (reg_device_id != 0x3001) {
        return -1;
    }

    /* Return success */
    return 0;
}

/**
 * 
 */
int opt3001::config_set(const enum opt3001_conversion_time ct) {
    int res;

    /* Enable automatic full scale
     * Set conversion time */
    uint16_t reg_config;
    res = register_read(OPT3001_REGISTER_CONFIG, &reg_config);
    if (res < 0) return -EIO;
    reg_config &= ~(0b1111 << 12);
    reg_config |= 0b1100 << 12;
    reg_config &= ~(0b1 << 11);
    reg_config |= (ct == OPT3001_CONVERSION_TIME_800MS) ? 0b1 : 0b0;
    res = register_write(OPT3001_REGISTER_CONFIG, reg_config);
    if (res < 0) return -EIO;

    /* Return success */
    return 0;
}

/**
 * 
 */
int opt3001::conversion_continuous_enable(void) {
    int res;

    /* Set continuous conversion mode */
    uint16_t reg_config;
    res = register_read(OPT3001_REGISTER_CONFIG, &reg_config);
    if (res < 0) return -EIO;
    reg_config &= ~(0b11 << 9);
    reg_config |= (0b11 << 9);
    res = register_write(OPT3001_REGISTER_CONFIG, reg_config);
    if (res < 0) return -EIO;

    /* Return success */
    return 0;
}

/**
 * 
 */
int opt3001::conversion_continuous_disable(void) {
    int res;

    /* Set shutdown conversion mode */
    uint16_t reg_config;
    res = register_read(OPT3001_REGISTER_CONFIG, &reg_config);
    if (res < 0) return -EIO;
    reg_config &= ~(0b11 << 9);
    reg_config |= (0b00 << 9);
    res = register_write(OPT3001_REGISTER_CONFIG, reg_config);
    if (res < 0) return -EIO;

    /* Return success */
    return 0;
}

/**
 * 
 */
int opt3001::conversion_singleshot_trigger(void) {
    int res;

    /* Set single-shot conversion mode */
    uint16_t reg_config;
    res = register_read(OPT3001_REGISTER_CONFIG, &reg_config);
    if (res < 0) return -EIO;
    reg_config &= ~(0b11 << 9);
    reg_config |= (0b01 << 9);
    res = register_write(OPT3001_REGISTER_CONFIG, reg_config);
    if (res < 0) return -EIO;

    /* Return success */
    return 0;
}

/**
 *
 * @param[out] lux
 * @return 0 in case of success, or a negative error code otherwise.
 */
int opt3001::lux_read(float *const lux) {
    int res;

    /* Read result register */
    uint16_t reg_result = 0x0000;
    res = register_read(OPT3001_REGISTER_RESULT, &reg_result);
    if (res < 0) {
        return -EIO;
    }

    /* Convert to float */
#if 0
    *lux = (float)(reg_result);
#else
    uint16_t mantissa = reg_result & 0x0FFF;
    uint16_t exponent = (reg_result & 0xF000) >> 12;
    *lux = mantissa * (0.01 * pow(2, exponent));
#endif

    /* Return success */
    return 0;
}
