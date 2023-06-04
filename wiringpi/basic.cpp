/* 
   basic.cpp: Basic VL53L5 example using Raspbery Pi WiringPi library

   Copyright (C) 2023 Simon D. Levy

   MIT license
*/

// #include <wiringPi.h>
#include <VL53L5cx.h>

// Set to 0 for polling
static const uint8_t INT_PIN = 4;

static const uint8_t LPN_PIN =  14;

// Set to 0 for continuous mode
static const uint8_t INTEGRAL_TIME_MS = 10;

static VL53L5cx _sensor(
        NULL, LPN_PIN, INTEGRAL_TIME_MS, VL53L5cx::RES_4X4_HZ_1);


int main(int argc, char ** argv)
{
    _sensor.begin();

    return 0;
}

void delay(const uint32_t msec)
{
}

void pinMode(const uint8_t pin, const uint8_t mode)
{
}

void digitalWrite(const uint8_t pin, const uint8_t value)
{
}

