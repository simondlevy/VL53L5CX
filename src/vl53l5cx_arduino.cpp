/*
*  VL53l5cx Arduino support
*
*  Copyright (c) 2021 Seth Bonn, Simon D. Levy
*
*  MIT License
*/

#include "vendor/vl53l5cx_i2.h"
#include "debugger.hpp"

#include <Arduino.h>
#include <Wire.h>

// Helper
static void start_transfer(TwoWire * wire, uint16_t rgstr)
{
    uint8_t buffer[2] {(uint8_t)(rgstr >> 8),
                       (uint8_t)(rgstr & 0xFF) }; 
    wire->write(buffer, 2);
}

// All these functions return 0 on success, nonzero on error



uint8_t VL53L1CX_ReadMulti(
        VL53L5CX_Platform *p_platform,
        uint16_t rgstr,
        uint8_t *data,
        uint32_t count)
{
    TwoWire * wire = (TwoWire *)p_platform->device;

    int status = 0;

    // Loop until the port is transmitted correctly
    do {
        wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F));

        start_transfer(wire, rgstr);

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
    if(count > 32) {
        while(i < count) {
            // If still more than 32 bytes to go, 32, else the remaining number
            // of bytes
            byte current_read_count = (count - i > 32 ? 32 : count - i); 
            wire->requestFrom(((uint8_t)((p_platform->address) & 0x7F)),
                    current_read_count);
            while (wire->available()) {
                data[i] = wire->read();
                i++;
            }
        }
    }
    else {
        wire->requestFrom(((uint8_t)((p_platform->address) & 0x7F)), count);
        while (wire->available()) {
            data[i] = wire->read();
            i++;
        }
    }
    
    return i != count;
}

uint8_t VL53L1CX_WriteMulti(
        VL53L5CX_Platform *p_platform,
        uint16_t rgstr,
        uint8_t *data,
        uint32_t count)
{
    TwoWire * wire = (TwoWire *)p_platform->device;

    // Partially based on https://github.com/stm32duino/VL53L1 VL53L1_I2CWrite()
    wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F)); 

    // Target register address for transfer
    start_transfer(wire, rgstr);
    for (uint32_t i = 0 ; i < count ; i++) 
    {
        wire->write(data[i]);
        if (i > 0 && i < count - 1 && i % 16 == 0) {
            // Flush buffer and end transmission completely
            wire->endTransmission(true);
            i++; // prepare for next byte

            // Restart send
            wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F)); 

            start_transfer(wire, rgstr+i);

            if (wire->write(data[i]) == 0) {
                Debugger::reportForever(
                        "VL53L1CX_WriteMulti failed to send %d bytes to regsiter 0x%02X",
                        count, rgstr);
            }
        }
    }

    return wire->endTransmission(true);
}
