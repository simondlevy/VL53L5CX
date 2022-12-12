/*******************************************************************************
  * Copyright (c) 2020, STMicroelectronics - All Rights Reserved
  *
  * This file is part of the VL53L5CX Ultra Lite Driver and is dual licensed,
  * either 'STMicroelectronics Proprietary license'
  * or 'BSD 3-clause "New" or "Revised" License' , at your option.
  *
  ********************************************************************************
  *
  * 'STMicroelectronics Proprietary license'
  *
  ********************************************************************************
  *
  * License terms: STMicroelectronics Proprietary in accordance with licensing
  * terms at www.st.com/sla0081
  *
  * STMicroelectronics confidential
  * Reproduction and Communication of this document is strictly prohibited unless
  * specifically authorized in writing by STMicroelectronics.
  *
  *
  ********************************************************************************
  *
  * Alternatively, the VL53L5CX Ultra Lite Driver may be distributed under the
  * terms of 'BSD 3-clause "New" or "Revised" License', in which case the
  * following provisions apply instead of the ones mentioned above :
  *
  ********************************************************************************
  *
  * License terms: BSD 3-clause "New" or "Revised" License.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  * 1. Redistributions of source code must retain the above copyright notice, this
  * list of conditions and the following disclaimer.
  *
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  *
  * 3. Neither the name of the copyright holder nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  *
  *******************************************************************************/

#include "platform.h"
#include "Debugger.h"

#include <Arduino.h>
#include <Wire.h>

static TwoWire * _wire;

// Called from VL53L5cx constructor
void setI2CDevice(void * dev)
{
    _wire = (TwoWire *)dev;
}


// Helper
static void start_transfer(uint16_t register_address)
{
    uint8_t buffer[2] {(uint8_t)(register_address >> 8),
                       (uint8_t)(register_address & 0xFF) }; 
    _wire->write(buffer, 2);
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
    // Partially based on https://github.com/stm32duino/VL53L1 VL53L1_I2CRead()

    int status = 0;

    // Loop until the port is transmitted correctly
    do {
        _wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F));

        start_transfer(RegisterAddress);

        status = _wire->endTransmission(false);

        // Fix for some STM32 boards
        // Reinitialize the i2c bus with the default parameters
#ifdef ARDUINO_ARCH_STM32
            if (status) {
                _wire->end();
                _wire->begin();
            }
#endif
        // End of fix

    } while (status != 0);

    uint32_t i = 0;
    if(size > 32)
    {
        while(i < size)
        {
            // If still more than 32 bytes to go, 32, else the remaining number
            // of bytes
            byte current_read_size = (size - i > 32 ? 32 : size - i); 
            _wire->requestFrom(((uint8_t)((p_platform->address) & 0x7F)),
                    current_read_size);
            while (_wire->available()) {
                p_values[i] = _wire->read();
                i++;
            }
        }
    }
    else
    {
        _wire->requestFrom(((uint8_t)((p_platform->address) & 0x7F)), size);
        while (_wire->available()) {
            p_values[i] = _wire->read();
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
    // Partially based on https://github.com/stm32duino/VL53L1 VL53L1_I2CWrite()
    _wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F)); 

    // Target register address for transfer
    start_transfer(RegisterAddress);
    for (uint32_t i = 0 ; i < size ; i++) 
    {
        _wire->write(p_values[i]);
        if (i > 0 && i < size - 1 && i % 16 == 0) {
            // Flush buffer and end transmission completely
            _wire->endTransmission(true);
            i++; // prepare for next byte

            // Restart send
            _wire->beginTransmission((uint8_t)((p_platform->address) & 0x7F)); 

            start_transfer(RegisterAddress+i);

            if (_wire->write(p_values[i]) == 0) {
                Debugger::reportForever(
                        "WrMulti failed to send %d bytes to regsiter 0x%02X",
                        size, RegisterAddress);
            }
        }
    }

    return _wire->endTransmission(true);
}

uint8_t Reset_Sensor(uint8_t lpn_pin)
{
    // Set pin LPN to LOW
    // (Set pin AVDD to LOW)
    // (Set pin VDDIO  to LOW)
    pinMode(lpn_pin, OUTPUT);
    digitalWrite(lpn_pin, LOW);
    delay(100);

    // Set pin LPN of to HIGH
    // (Set pin AVDD of to HIGH)
    // (Set pin VDDIO of  to HIGH)
    digitalWrite(lpn_pin, HIGH);
    delay(100);

    return 0;
}

void SwapBuffer( uint8_t   *buffer, uint16_t     size) {

    // Example of possible implementation using <string.h>
    for(uint32_t i = 0; i < size; i = i + 4) {

        uint32_t tmp = (
                buffer[i]<<24)
            |(buffer[i+1]<<16)
            |(buffer[i+2]<<8)
            |(buffer[i+3]);

        memcpy(&(buffer[i]), &tmp, 4);
    }
} 

uint8_t WaitMs(
        VL53L5CX_Platform *p_platform,
        uint32_t TimeMs)
{
    delay(TimeMs);

    return 0;
}
