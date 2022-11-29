/*
*  VL53L5CX resolution test
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include <Wire.h>
#include "VL53L5cx.h"

static const uint8_t LPN_PIN = 5;

// Choices are RESOLUTION_4X4 and RESOLUTION_8x8
static VL53L5cx::resolution_t RESOLUTION = VL53L5cx::RESOLUTION_8X8;
//static VL53L5cx::resolution_t RESOLUTION = VL53L5cx::RESOLUTION_4X4;

static VL53L5cx sensor = VL53L5cx(LPN_PIN, RESOLUTION); 

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
    Serial.println("Starting sensor; may take a few seconds ...");
    sensor.begin();

} // setup

void loop(void)
{
    // Use polling function to know when a new measurement is ready.
    if (sensor.isReady()) {

        sensor.collectData();

        for (uint8_t i=SIZE; i>0; i--) {

            for (uint8_t j=SIZE; j>0; j--) {

                uint8_t d = sensor.getDistance((i-1)*SIZE+j-1);
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
