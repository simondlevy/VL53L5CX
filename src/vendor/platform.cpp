/*
*  VL53l5cx Arduino support
*
*  Copyright (c) 2021 Seth Bonn, Simon D. Levy
*
*  MIT License
*/

#include "vl53l5cx_i2.h"
#include "Debugger.h"

#include <Arduino.h>
#include <Wire.h>

// Helper
static void start_transfer(TwoWire * wire, uint16_t register_address)
{
    uint8_t buffer[2] {(uint8_t)(register_address >> 8),
                       (uint8_t)(register_address & 0xFF) }; 
    wire->write(buffer, 2);
}

// All these functions return 0 on success, nonzero on error

uint8_t RdByte(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t *p_value)
{

    uint8_t res = RdMulti(p_platform, RegisterAddress, p_value, 1);

    return res;
}

uint8_t RdMulti(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t *p_values,
        uint32_t size)
{
    TwoWire * wire = (TwoWire *)p_platform->device;

    int status = 0;

    // Loop until the port is transmitted correctly
    do {
        wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F));

        start_transfer(wire, RegisterAddress);

        status = wire->endTransmission(false);

        // Fix for some STM32 boards
        // Reinitialize the i2c bus with the default parameters
#ifdef ARDUINO_ARCH_STM32
            if (status) {
                wire->end();
                wire->begin();
            }
#endif
        // End of fix

    } while (status != 0);

    uint32_t i = 0;
    if(size > 32) {
        while(i < size) {
            // If still more than 32 bytes to go, 32, else the remaining number
            // of bytes
            byte current_read_size = (size - i > 32 ? 32 : size - i); 
            wire->requestFrom(((uint8_t)((p_platform->address) & 0x7F)),
                    current_read_size);
            while (wire->available()) {
                p_values[i] = wire->read();
                i++;
            }
        }
    }
    else {
        wire->requestFrom(((uint8_t)((p_platform->address) & 0x7F)), size);
        while (wire->available()) {
            p_values[i] = wire->read();
            i++;
        }
    }
    
    return i != size;
}

uint8_t WrByte(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t value)
{
    // Just use WrMulti but 1 byte
    uint8_t res = WrMulti(p_platform, RegisterAddress, &value, 1); 
    return res;
}

uint8_t WrMulti(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t *p_values,
        uint32_t size)
{
    TwoWire * wire = (TwoWire *)p_platform->device;

    // Partially based on https://github.com/stm32duino/VL53L1 VL53L1_I2CWrite()
    wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F)); 

    // Target register address for transfer
    start_transfer(wire, RegisterAddress);
    for (uint32_t i = 0 ; i < size ; i++) 
    {
        wire->write(p_values[i]);
        if (i > 0 && i < size - 1 && i % 16 == 0) {
            // Flush buffer and end transmission completely
            wire->endTransmission(true);
            i++; // prepare for next byte

            // Restart send
            wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F)); 

            start_transfer(wire, RegisterAddress+i);

            if (wire->write(p_values[i]) == 0) {
                Debugger::reportForever(
                        "WrMulti failed to send %d bytes to regsiter 0x%02X",
                        size, RegisterAddress);
            }
        }
    }

    return wire->endTransmission(true);
}
