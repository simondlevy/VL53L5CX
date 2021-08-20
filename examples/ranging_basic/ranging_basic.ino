/*
*  VL53L5CX ULD basic example    
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include <Wire.h>

#include "Debugger.hpp"
#include "vl53l5cx_api.h"

static VL53L5CX_Configuration Dev = {};  // Sensor configuration
static VL53L5CX_ResultsData Results = {};  // Results data from VL53L5CX

static const uint8_t LPN_PIN = 3;

void setup(void)
{
    // Start I^2C
    Wire.begin();

    // Start serial debugging
    Serial.begin(115200);

    delay(1000);

    Debugger::printf("starting\n\n");

    delay(1000);

    // Fill the platform structure with customer's implementation. For this
    // example, only the I2C address is used.
    Dev.platform.address = 0x52; // 0x29;

    // Reset the sensor by toggling the LPN pin
    Reset_Sensor(LPN_PIN);

    // (Optional) Check if there is a VL53L5CX sensor connected
    uint8_t isAlive = 0;
    uint8_t error = vl53l5cx_is_alive(&Dev, &isAlive);
    if(!isAlive || error) {
        Debugger::reportForever("VL53L5CX not detected at requested address");
    }

    // (Mandatory) Init VL53L5CX sensor
    error = vl53l5cx_init(&Dev);
    if(error) {
        Debugger::reportForever("VL53L5CX ULD Loading failed");
    }

    Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

    error = vl53l5cx_start_ranging(&Dev);
}

void loop(void)
{
    Serial.println(millis());

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
    }
}
