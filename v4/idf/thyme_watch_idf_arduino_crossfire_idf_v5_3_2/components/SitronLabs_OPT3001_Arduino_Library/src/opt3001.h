#ifndef OPT3001_H
#define OPT3001_H

/* Arduino libraries */
#include <Arduino.h>
#include <Wire.h>

/* C/C++ libraries */
#include <errno.h>
#include <stdint.h>

/**
 * 
 */
enum opt3001_register {
    OPT3001_REGISTER_RESULT = 0x00,
    OPT3001_REGISTER_CONFIG = 0x01,
    OPT3001_REGISTER_LIMITL = 0x02,
    OPT3001_REGISTER_LIMITH = 0x03,
    OPT3001_REGISTER_MANUID = 0x7E,
    OPT3001_REGISTER_DEVIID = 0x7F,
};

/**
 *
 */
enum opt3001_conversion_time {
    OPT3001_CONVERSION_TIME_100MS,
    OPT3001_CONVERSION_TIME_800MS,
};

/**
 * 
 */
class opt3001 {
   public:
    int setup(TwoWire &i2c_library, const uint8_t i2c_address);
    int detect(void);
    int config_set(const enum opt3001_conversion_time ct);
    int conversion_continuous_enable(void);
    int conversion_continuous_disable(void);
    int conversion_singleshot_trigger(void);
    int register_read(const enum opt3001_register reg_address, uint16_t *const reg_content);
    int register_write(const enum opt3001_register reg_address, const uint16_t reg_content);
    int lux_read(float *const lux);

   protected:
    TwoWire *m_i2c_library = NULL;
    uint8_t m_i2c_address;
};

#endif
