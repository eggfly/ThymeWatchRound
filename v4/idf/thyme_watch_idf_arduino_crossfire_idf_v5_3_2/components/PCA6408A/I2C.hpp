/**
 * @file
 *    I2C_HPP
 *
 * @brief
 *    This header file is part of a library that facilitates I2C communication for Arduino-based systems, specifically tailored for the ESP32.
 *    It provides functions for initializing the I2C bus, reading and writing bytes and byte arrays to and from I2C devices.
 *
 *    Key Features:
 *    - Support for Standard Mode (100 kHz) and Fast Mode (400 kHz) I2C communication.
 *    - Functions for reading and writing single bytes and arrays of bytes over I2C.
 *    - Customizable SDA and SCL pins and clock frequency.
 *
 *    This library abstracts the complexities of I2C communication, making it simpler to interface with I2C devices.
 *    It is ideal for projects requiring communication with sensors, displays, and other peripherals using the I2C protocol.
 *
 * Example Usage:
 *    i2cBegin(ESP32_I2C_SDA, ESP32_I2C_SCL, SCL_CLOCK_FREQUENCY);  // Initialize I2C
 *    uint8_t data = i2cReadByte(DEVICE_ADDRESS, REGISTER_ADDRESS); // Read a byte from a device
 *    i2cWriteByte(DEVICE_ADDRESS, REGISTER_ADDRESS, data);         // Write a byte to a device
 *
 * @author
 *    Itay Nave, Embedded Software Engineer
 * @date
 *    12/25/2023
 *
 * @copyright
 *    © 2023 Itay Nave. All rights reserved.
 *    This software is provided "as is", without warranty of any kind, express or implied.
 *    Redistribution and use in source and binary forms, with or without modification,
 *    are permitted provided that the above copyright notice and this permission notice
 *    appear in all copies.
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>

#define SCL_CLOCK_FREQUENCY 100000 // (100 kHz (Standard Mode) || 400 kHz (Fast Mode))

bool i2cBegin(uint8_t sda, uint8_t scl, uint32_t frequency);

bool i2cReadByte(uint8_t address, uint8_t subAddress, uint8_t *data);
void i2cWriteByte(uint8_t address, uint8_t subAddress, uint8_t data);

void i2cReadBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t *dest);
void i2cWriteBytes(uint8_t address, uint8_t subAddress, uint8_t count, const uint8_t *data);
