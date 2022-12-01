/*
*  VL53L5CX ULD basic example    
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include <Wire.h>
#include "Debugger.hpp"
#include "VL53L5cx.h"

static VL53L5cx sensor = VL53L5cx(14); // LPN pin

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

        for(uint8_t i=0; i<16; i++) {

            Debugger::printf(
                    "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                    i,
                    sensor.getTargetStatus(i),
                    sensor.getDistance(i));
        }
        Debugger::printf("\n");
    }

    delay(5);
}
