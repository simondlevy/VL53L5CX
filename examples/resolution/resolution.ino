/*
*  VL53L5CX resolution test
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include <Wire.h>
#include "VL53L5cx.h"

// Choices are RESOLUTION_4X4 and RESOLUTION_8x8
static VL53L5cx::resolution_t RESOLUTION = VL53L5cx::RESOLUTION_8X8;

static VL53L5cx sensor = VL53L5cx(5, // LPN pin
                                  0x29, // I^2C address
                                  RESOLUTION); 

// Determine image size from resolution
static const uint8_t SIZE = RESOLUTION == VL53L5cx::RESOLUTION_8X8 ? 8 : 4;

void setup(void)
{
    // Start I^2C
    Wire.begin();

    delay(100);

    // Start serial debugging
    Serial.begin(115200);

    // Start sensor
    sensor.begin();

} // setup

void loop(void)
{
    // Use polling function to know when a new measurement is ready.
    if (sensor.isReady()) {

        for (uint8_t i=0; i<SIZE; i++) {

            for (uint8_t j=0; j<SIZE; j++) {

                uint8_t d = sensor.getDistance(i*SIZE+j);
                Serial.print(d < 10 ? "  " : d < 100 ? " " : ""); // right-justify
                Serial.print(d);
                Serial.print(" ");
            }

            Serial.println();
        }

        Serial.println();

        delay(100);
    }
}
