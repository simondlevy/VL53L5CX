/*
 * This example shows the possibility of VL53L5CX to get/set params. It
 * initializes the VL53L5CX ULD, set a configuration, and starts
 * a ranging to capture 10 frames.
 *
 *  Copyright (c) 2021 Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "Debugger.hpp"
#include "vl53l5cx_api.h"

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
    if (!isAlive || error) {
        Debugger::reportForever("VL53L5CX not detected at requested address");
    }

    // Init VL53L5CX sensor
    error = vl53l5cx_init(&Dev);
    if (error) {
        Debugger::reportForever("VL53L5CX ULD Loading failed");
    }

    Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

    error = vl53l5cx_start_ranging(&Dev);
    if (error !=0) {
        Debugger::printf("start error = 0x%02X", error);
    }

} // setup


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

            // As the sensor is set in 4x4 mode by default, we have a total
            // of 16 zones to print */
            Debugger::printf("Print data no : %3u\n", Dev.streamcount);
            for (uint8_t i = 0; i < 16; i++) {

                // Print per zone results. These results are the same for all targets 
                Debugger::printf("Zone %3u : %2u, %6lu, %6lu, ",
                        i,
                        Results.nb_target_detected[i],
                        Results.ambient_per_spad[i],
                        Results.nb_spads_enabled[i]);

                for (uint8_t j = 0; j < VL53L5CX_NB_TARGET_PER_ZONE; j++)
                {
                    // Print per target results. These results depends of the target nb
                    uint16_t idx = VL53L5CX_NB_TARGET_PER_ZONE * i + j;
                    Debugger::printf("Target[%1u] : %2u, %4d, %6lu, %3u, ",
                            j,
                            Results.target_status[idx],
                            Results.distance_mm[idx],
                            Results.signal_per_spad[idx],
                            Results.range_sigma_mm[idx]);
                }
                Debugger::printf("\n");
            }
            Debugger::printf("\n");
            loop_count++;
        }

        // Wait a few ms to avoid too high polling (function in platform
        // file, not in API) 
        WaitMs(&(Dev.platform), 5);
    }

    else if (loop_count == 10) {
        vl53l5cx_stop_ranging(&Dev);
        loop_count++;
    }
    else {
        Debugger::printf("End of ULD demo\n");
    }

} // loop
