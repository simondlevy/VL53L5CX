/*
 * This example shows the possibility using VL53L5CX to calibrate Xtalk. It
 * initializes the VL53L5CX ULD, perform a Xtalk calibration, and starts
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

    // Start Xtalk calibration with a 3% reflective target at 600mm for the
    // sensor, using 4 samples.
    Debugger::printf("Running Xtalk calibration...\n");
    sensor.calibrateXtalk(3, 4, 600);
    Debugger::printf("Xtalk calibration done\n");


    // Get Xtalk calibration data, in order to use them later
    VL53L5cx::XtalkCalibrationData xtalk_data;
    sensor.getXtalkCalibrationData(xtalk_data);

    // Set Xtalk calibration data
    sensor.setXtalkCalibrationData(xtalk_data);

    sensor.begin();
}

void loop(void)
{
    static uint8_t loop_count;

    if (loop_count < 10) {

        // Use polling function to know when a new measurement is ready.
        if (sensor.isReady()) {

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

