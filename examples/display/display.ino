/*
*  VL53L5CX 8x8 display example.  Sends 64-byte messaages over serial to be
*  displayed by a program (e.g, OpenCV) on the host computer.
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include <Wire.h>
#include "VL53L5cx.h"

static VL53L5cx sensor = VL53L5cx(5, 0x29, VL53L5cx::RESOLUTION_8X8);

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

        for (uint8_t i=0; i<64; i++) {

            Serial.write(sensor.getDistance(i));
        }

        Serial.write(0xFF); // sentinel byte
    }
}
