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
    static uint8_t loop_count;

    if (loop_count < 10) {

        // Use polling function to know when a new measurement is ready.
        if (sensor.isReady()) {

            sensor.collectData();

            // As the sensor is set in 4x4 mode by default, we have a total of
            // 16 zones to print. For this example, only the data of first zone
            // are printed.
            Debugger::printf("Print data no : %3u\n", sensor.getStreamCount());

            for(uint8_t i=0; i<16; i++) {

                Debugger::printf(
                        "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        sensor.getTargetStatus(i),
                        sensor.getDistance(i));
            }
            Debugger::printf("\n");
            loop_count++;
        }
    }

    else if (loop_count == 10) {

        // Wait a few ms to avoid too high polling (function in platform
        // file, not in API)
        delay(5);
        sensor.stop();
        loop_count++;
    }

    else {
        Debugger::printf("End of ULD demo\n");
        delay(500);
    }
}
