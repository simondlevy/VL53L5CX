#include "platform.h"

#include <Arduino.h>
#include <Wire.h>

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
    // Partially based on https://github.com/stm32duino/VL53L1 VL53L1_I2CRead() function

    int status = 0;
    //Loop until the port is transmitted correctly
    do {
        Wire.beginTransmission((uint8_t)((p_platform->address) & 0x7F));

        const uint8_t buffer[2] {RegisterAddress >> 8, RegisterAddress & 0xFF };
        Wire.write(buffer, 2);
        status = Wire.endTransmission(false);
        //Fix for some STM32 boards
        //Reinitialize th i2c bus with the default parameters
#ifdef ARDUINO_ARCH_STM32
            if (status) {
            Wire.end();
            Wire.begin();
            }
#endif
        //End of fix
    } while (status != 0);

    int i = 0;
    if(size > 32)
    {
        Serial.println("\nLarge read request, size = " + size);
        while(i < size)
        {
            byte current_read_size = (size - i > 32 ? 32 : size - i); // If still more than 32 bytes to go, 32, else the remaining number of bytes
            Wire.requestFrom(((uint8_t)((p_platform->address) & 0x7F)), current_read_size);
            while (Wire.available()) {
                p_values[i] = Wire.read();
                i++;
            }
        }
    }
    else
    {
        Wire.requestFrom(((uint8_t)((p_platform->address) & 0x7F)), size);
        while (Wire.available()) {
            p_values[i] = Wire.read();
            i++;
        }
    }
    
    return i!=size;
}

uint8_t WrByte(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t value)
{
    uint8_t res = WrMulti(p_platform, RegisterAddress, &value, 1); // Just use WrMulti but 1 byte
    return res;
}

uint8_t WrMulti(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t *p_values,
        uint32_t size)
{
    // Partially based on https://github.com/stm32duino/VL53L1 VL53L1_I2CWrite() function

    Wire.beginTransmission((uint8_t)((p_platform->address) & 0x7F)); 

    uint8_t buffer[2] {RegisterAddress >> 8, RegisterAddress & 0xFF }; // Target register address for transfer
    Wire.write(buffer, 2); // Write register address

    for (int i = 0 ; i < size ; i++) 
    {
        if(Wire.write(p_values[i]) == 0) // If this returns 0, the write was not successful due to buffer being full -> flush buffer and keep going
        {
            Wire.endTransmission(false); // Flush buffer but do not send stop bit so we can keep going
            Wire.beginTransmission((uint8_t)((p_platform->address) & 0x7F)); // Restart send

            buffer[0] = (RegisterAddress+i) >> 8; // Adjust target register address
            buffer[1] = (RegisterAddress+i) & 0xFF;
            Wire.write(buffer, 2); // Send new register address to keep going from
            if(Wire.write(p_values[i]) == 0)
            {
                // Error handling, resend failed!
            }
        }
    }

    uint8_t res = Wire.endTransmission(true);

    return res;
}

uint8_t Reset_Sensor(uint8_t lpn_pin)
{
    uint8_t status = 0;

    /* (Optional) XXX This function returns 0 if OK */

    /* Set pin LPN to LOW */
    /* Set pin AVDD to LOW */
    /* Set pin VDDIO  to LOW */
    pinMode(lpn_pin, OUTPUT);
    digitalWrite(lpn_pin, LOW);
    delay(100);

    /* Set pin LPN of to HIGH */
    /* Set pin AVDD of to HIGH */
    /* Set pin VDDIO of  to HIGH */
    digitalWrite(lpn_pin, HIGH);
    delay(100);

    return status;
}

void SwapBuffer( uint8_t   *buffer, uint16_t     size) {
    uint32_t i, tmp;

    /* Example of possible implementation using <string.h> */
    for(i = 0; i < size; i = i + 4) 
    {
        tmp = (
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
    /* XXX This function returns 0 if OK */

    delay(TimeMs);

    return 0;
}
