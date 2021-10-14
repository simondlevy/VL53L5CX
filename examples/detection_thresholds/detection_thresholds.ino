/*
 * This example shows the possibility of VL53L5CX to program detection thresholds. It
 * initializes the VL53L5CX ULD, create 2 thresholds per zone for a 4x4 resolution,
 * and starts a ranging to capture 10 frames.

 * In this example, we also suppose that the number of target per zone is
 * set to 1 , and all output are enabled (see file platform.h).
 *
 *  Copyright (c) 2021 Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "Debugger.hpp"
#include "VL53L5cx.h"

static const uint8_t INT_PIN = 8;
static const uint8_t LPN_PIN = 5;

static VL53L5cx::detection_thresholds_t thresholds =
{
        150,  // min kcps/spads
        150,  // max kcps/spads
        200,  // min distance mm
        400   // max distance mm
};

static VL53L5cx sensor = VL53L5cx(
        LPN_PIN, 
        0x29,  // device address
        VL53L5cx::RESOLUTION_4X4, 
        VL53L5cx::TARGET_ORDER_CLOSEST,
        1);    // ranging frequency 


static volatile bool VL53L5_intFlag;
static void VL53L5_intHandler(void)
{
    VL53L5_intFlag = true;
}

void setup (void)
{
    // Start I^2C
    Wire.begin();

    delay(100);

    // Start serial debugging
    Serial.begin(115200);

    // Set up interrupt
    pinMode(INT_PIN, INPUT); 
    attachInterrupt(INT_PIN, VL53L5_intHandler, FALLING);

    Serial.println("Starting sensor ...");
    sensor.begin(thresholds);
    Serial.println("Put an object between 200mm and 400mm to catch an interrupt\n");
    delay(2000);

} // setup

void loop(void)
{
    static uint32_t loop_count;

    if (loop_count < 100) {

        if (VL53L5_intFlag) {

            VL53L5_intFlag = false;

            sensor.collectData();

            // As the sensor is set in 4x4 mode by default, we have a total
            // of 16 zones to print. For this example, only the data of
            // first zone are print
            Debugger::printf("Print data no : %3u\n", sensor.getStreamCount());
            for (uint8_t i = 0; i < 16; i++)
            {
                Debugger::printf("Zone : %3d, Status : %3u, Distance : %4d mm, Signal : %5lu kcps/SPADs\n",
                        i,
                        sensor.getTargetStatus(i),
                        sensor.getDistance(i),
                        sensor.getSignalPerSpad(i));
            }
            Debugger::printf("\n");
            loop_count++;
        }

        // Wait a few ms to avoid too high polling (function in platform file, not in API) 
        delay(5);
    }

    else if (loop_count == 100) {
        sensor.stop(); 
        loop_count++;
    }

    else {
        Debugger::printf("End of ULD demo\n");
        delay(500);
    }

} // loop
