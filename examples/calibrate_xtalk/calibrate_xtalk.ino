/*
 * This example shows the possibility of VL53L5CX to calibrate Xtalk. It
 * initializes the VL53L5CX ULD, perform a Xtalk calibration, and starts
 * a ranging to capture 10 frames.

 * In this example, we also suppose that the number of target per zone is
 * set to 1 , and all output are enabled (see file platform.h).
 *
 *  Copyright (c) 2021 Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "Debugger.hpp"
#include "vl53l5cx_api.h"
#include "vl53l5cx_plugin_xtalk.h"

static const uint8_t LPN_PIN = 5;

static VL53L5CX_Configuration Dev = {};  // Sensor configuration
static VL53L5CX_ResultsData Results = {};  // Results data from VL53L5CX

void setup(void)
{
    // Start I^2C
    Wire.begin();

    delay(100);

    // Start serial debugging
    Serial.begin(115200);

    // Fill the platform structure with customer's implementation. For this
    // example, only the I2C address is used.
    Dev.platform.address = 0x29;

    // Reset the sensor by toggling the LPN pin
    Reset_Sensor(LPN_PIN);

    // Make sure there is a VL53L5CX sensor connected
    uint8_t isAlive = 0;
    uint8_t error = vl53l5cx_is_alive(&Dev, &isAlive);
    if(!isAlive || error) {
        Debugger::reportForever("VL53L5CX not detected at requested address");
    }

    // Init VL53L5CX sensor
    error = vl53l5cx_init(&Dev);
    if(error) {
        Debugger::reportForever("VL53L5CX ULD Loading failed");
    }

    Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

    error = vl53l5cx_start_ranging(&Dev);
    if(error !=0) {
        Debugger::printf("start error = 0x%02X", error);
    }

    // Start Xtalk calibration with a 3% reflective target at 600mm for the
    // sensor, using 4 samples.
    Debugger::printf("Running Xtalk calibration...\n");

    error = vl53l5cx_calibrate_xtalk(&Dev, 3, 4, 600);

    if (error) {
        Debugger::reportForever("vl53l5cx_calibrate_xtalk failed, status %u\n", error);
    }
    else {

       uint8_t xtalk_data[VL53L5CX_XTALK_BUFFER_SIZE] = {};

        Debugger::printf("Xtalk calibration done\n");

        // Get Xtalk calibration data, in order to use them later
        vl53l5cx_get_caldata_xtalk(&Dev, xtalk_data);

        // Set Xtalk calibration data
        vl53l5cx_set_caldata_xtalk(&Dev, xtalk_data);
    }

    vl53l5cx_start_ranging(&Dev);
}

void loop(void)
{
    static uint8_t loop_count;

    if (loop_count < 10) {

        // Use polling function to know when a new measurement is ready.
        // Another way can be to wait for HW interrupt raised on PIN A3
        // (GPIO 1) when a new measurement is ready 

        uint8_t isReady = 0;
        vl53l5cx_check_data_ready(&Dev, &isReady);

        if (isReady) {

            vl53l5cx_get_ranging_data(&Dev, &Results);

            // As the sensor is set in 4x4 mode by default, we have a total of
            // 16 zones to print. For this example, only the data of first zone
            // are printed 
            Debugger::printf("Print data no : %3u\n", Dev.streamcount);
            for(uint8_t i = 0; i < 16; i++) {
                Debugger::printf(
                        "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
            loop_count++;
        }
    }

    else if (loop_count == 10) {

        // Wait a few ms to avoid too high polling (function in platform
        // file, not in API)
        delay(5);
        vl53l5cx_stop_ranging(&Dev);
        loop_count++;
    }

    else {
        Debugger::printf("End of ULD demo\n");
        delay(500);
    }
}

