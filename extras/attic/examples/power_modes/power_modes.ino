/* 
* This example shows the possibility of VL53L5CX to change power mode. It
* initializes the VL53L5CX ULD, set a configuration, change the power mode, and
* starts a ranging to capture 10 frames.
*
* In this example, we also suppose that the number of target per zone is
* set to 1 , and all output are enabled (see file platform.h).
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include "Wire.h"

#include "Debugger.hpp"
#include "st/vl53l5cx_api.h"

static const uint8_t LPN_PIN = 5;

static VL53L5CX_Configuration Dev = {};  // Sensor configuration

void setup(void)
{
    // Start I^2C
    Wire.begin();

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

    //  For the example, we don't want to use the sensor during 10 seconds. In order to reduce
    // the power consumption, the sensor is set to low power mode.
    error = vl53l5cx_set_power_mode(&Dev, VL53L5CX_POWER_MODE_SLEEP);
    if(error) {
        Debugger::reportForever("vl53l5cx_set_power_mode failed, status %u\n", error);
    }

    Debugger::printf("VL53L5CX is now sleeping\n");

    //  We wait 5 seconds, only for the example
    Debugger::printf("Waiting 5 seconds for the example...\n");
    WaitMs(&(Dev.platform), 5000);

    //  After 5 seconds, the sensor needs to be restarted
    error = vl53l5cx_set_power_mode(&Dev, VL53L5CX_POWER_MODE_WAKEUP);
    if(error) {
        Debugger::reportForever("vl53l5cx_set_power_mode failed, status %u\n", error);
    }
    Debugger::printf("VL53L5CX is now waking up\n");

    vl53l5cx_start_ranging(&Dev);
}

void loop(void)
{
    static uint8_t loop_count;

    if (loop_count < 10) {

        //  Use polling function to know when a new measurement is ready.
        //  Another way can be to wait for HW interrupt raised on PIN A3
        // (GPIO 1) when a new measurement is ready

        uint8_t isReady = 0;
        vl53l5cx_check_data_ready(&Dev, &isReady);

        if (isReady) {

            VL53L5CX_ResultsData Results = {};
            vl53l5cx_get_ranging_data(&Dev, &Results);

            //  As the sensor is set in 4x4 mode by default, we have a total of
            //  16 zones to print. For this example, only the data of first
            //  zone are printed
            Debugger::printf("Print data no : %3u\n", Dev.streamcount);
            for(uint8_t i = 0; i < 16; i++) {
                Debugger::printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
            loop_count++;
        }

        //  Wait a few ms to avoid too high polling (function in platform
        // file, not in API)
        WaitMs(&(Dev.platform), 5);
    }

    else if (loop_count == 10) {
        vl53l5cx_stop_ranging(&Dev);
        loop_count++;
    }

    else { 
	printf("End of ULD demo\n");
    }
}
