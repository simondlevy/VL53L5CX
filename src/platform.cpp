#include "platform.h"

#include <Arduino.h>
#include <Wire.h>

// All these functions return 0 on success, nonzero on error

uint8_t RdByte(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t *p_value)
{
    uint8_t DeviceAddress = p_platform->address;

    Wire.beginTransmission(DeviceAddress);
    Wire.write(RegisterAddress >> 8); //MSB
    Wire.write(RegisterAddress & 0xFF); //LSB
    if (Wire.endTransmission() != 0) { //Send restart cmd w/o releasing bus
        return 1; //Sensor did not ACK
    }

    Wire.requestFrom(DeviceAddress, (uint8_t)1);
    if (Wire.available()) {
        *p_value = Wire.read();
        return 0;
    }

    return 1; //Error: Sensor did not respond
}

uint8_t RdMulti(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t *p_values,
        uint32_t size)
{
    uint8_t DeviceAddress = p_platform->address;

    Wire.beginTransmission(DeviceAddress);  

    Wire.write(RegisterAddress >> 8); //MSB
    Wire.write(RegisterAddress & 0xFF); //LSB

    uint8_t status = Wire.endTransmission(false); 
    if (status) { // failed
        return status;
    }
    uint32_t i = 0;
    if (Wire.requestFrom(DeviceAddress, size) != size) {
        return 1; // failed
    }

    while (Wire.available()) {
        p_values[i++] = Wire.read();
    }
    
    return 0;
}

uint8_t WrByte(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t value)
{
    uint8_t DeviceAddress = p_platform->address;

    Wire.beginTransmission(DeviceAddress);
    Wire.write(RegisterAddress >> 8); //MSB
    Wire.write(RegisterAddress & 0xFF); //LSB
    Wire.write(value);
    return (Wire.endTransmission() != 0);
}

uint8_t WrMulti(
        VL53L5CX_Platform *p_platform,
        uint16_t RegisterAddress,
        uint8_t *p_values,
        uint32_t size)
{
    uint8_t DeviceAddress = p_platform->address;

    Wire.beginTransmission(DeviceAddress);    // Initialize the Tx buffer

    Wire.write(RegisterAddress >> 8); //MSB
    Wire.write(RegisterAddress & 0xFF); //LSB

    Wire.write(p_values, size);

    return Wire.endTransmission() != 0; // Send the Tx buffer
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
