/*
 * This example shows the possibility of VL53L5CX to multiple targets per
 * zone.  initializes the VL53L5CX ULD, set a configuration, and starts a
 * ranging to capture 10 frames.
 *
 *  Copyright (c) 2021 Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "Debugger.hpp"
#include "VL53L5cx.h"

static VL53L5cx sensor = VL53L5cx(5); // LPN pin

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
    static uint8_t loop_count;

    if (loop_count < 10) {

        if (sensor.isReady()) {

            // As the sensor is set in 4x4 mode by default, we have a total
            // of 16 zones to print */
            Debugger::printf("Print data no : %3u\n", sensor.getStreamCount());
            for (uint8_t i = 0; i < 16; i++) {

                // Print per zone results. These results are the same for all targets 
                Debugger::printf("Zone %3u : %2u, %6lu, %6lu, ",
                        i,
                        sensor.getNbTargetDetected(i),
                        sensor.getAmbientPerSpad(i),
                        sensor.getNbSpadsEnabled(i));

                for (uint8_t j = 0; j < VL53L5cx::NB_TARGET_PER_ZONE; j++)
                {
                    // Print per target results. These results depend on the target nb
                    Debugger::printf("Target[%1u] : %2u, %4d, %6lu, %3u, ",
                            j,
                            sensor.getTargetStatus(i, j),
                            sensor.getDistance(i, j),
                            sensor.getSignalPerSpad(i, j),
                            sensor.getRangeSigma(i, j));
                }
                Debugger::printf("\n");
            }
            Debugger::printf("\n");
            loop_count++;
        }

        // Wait a few ms to avoid too high polling (function in platform
        // file, not in API) 
        delay(5);
    }

    else if (loop_count == 10) {
        delay(5);
        sensor.stop();
        loop_count++;
    }
    else {
        Debugger::printf("End of ULD demo\n");
        delay(500);
    }

} // loop
