/*
 *
 * This example shows the differences between ranging modes of VL53L5CX
 * (mode continuous and autonomous). For both modes, it initializes the VL53L5CX
 * ULD, set the mode, and starts a ranging to capture 10 frames.
 *
 * In this example, we also suppose that the number of target per zone is
 * set to 1 , and all output are enabled (see file platform.h).
 *
 *
 *  Copyright (c) 2021 Simon D. Levy
 *
 *  MIT License
*/ 


#include "Wire.h"

#include "Debugger.hpp"
#include "vl53l5cx_api.h"

static const uint8_t LPN_PIN = 3;

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

    // Set ranging mode autonomous  
    uint8_t status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
    if(status) {
        Debugger::reportForever("vl53l5cx_set_ranging_mode failed, status %u\n", status);
    }

    // Using autonomous mode, the integration time can be updated (not possible
    // using continuous)
    vl53l5cx_set_integration_time_ms(&Dev, 20);

    error = vl53l5cx_start_ranging(&Dev);
    if(error !=0) {
        Debugger::printf("start error = 0x%02X", error);
    }

} // setup

static uint8_t testAutonomousMode(uint8_t loop_count) 
{
    uint8_t isReady = 0;
    vl53l5cx_check_data_ready(&Dev, &isReady);

    if (isReady) {

        VL53L5CX_ResultsData Results = {};

        vl53l5cx_get_ranging_data(&Dev, &Results);

        // As the sensor is set in 4x4 mode by default, we have a total
        // of 16 zones to print. For this example, only the data of first zone are
        // printed
        Debugger::printf("Print data no : %3u\n", Dev.streamcount);

        for (uint8_t i = 0; i < 16; i++) {

            Debugger::printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                    i,
                    Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                    Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
        }

        Debugger::printf("\n");
        loop_count++;
    }

    // Wait a few ms to avoid too high polling (function in platform
    // file, not in API)
    WaitMs(&(Dev.platform), 5);

    return loop_count;
}

void loop(void)
{
    static uint8_t loop_count;

    if (loop_count < 10) {
        loop_count = testAutonomousMode(loop_count);
    }

    if (loop_count == 10) {
        vl53l5cx_stop_ranging(&Dev);
        Debugger::printf("Stop ranging autonomous\n");
        loop_count++;
    }

    if (loop_count == 11) {

        // Set ranging mode continuous   
        // In continuous mode, the integration time cannot be programmed
        // (automatically set to maximum value)
        uint8_t error = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_CONTINUOUS);
        if (error) {
            Debugger::reportForever("vl53l5cx_set_ranging_mode failed, status %u\n", error);
        }

        // Trying to update value below will have no impact on integration time 
        //status = vl53l5cx_set_integration_time_ms(&Dev, 20);

        loop_count++;
    }

    /*
    // Start a ranging session 
    status = vl53l5cx_start_ranging(&Dev);
    Debugger::printf("Start ranging continuous\n");

    if (loop_count >= 10 && loop_count < 20) {

        status = vl53l5cx_check_data_ready(&Dev, &isReady);

        if (isReady) {

            vl53l5cx_get_ranging_data(&Dev, &Results);

            // As the sensor is set in 4x4 mode by default, we have a total
            // of 16 zones to print 
            Debugger::printf("Print data no : %3u\n", Dev.streamcount);
            for (uint8_t i = 0; i < 16; i++) {

                Debugger::printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
            loop_count++;
        }

        // Wait a few ms to avoid too high polling (function in platform
        // file, not in API)
        WaitMs(&(Dev.platform), 5);
    }

    if (loop_count == 20) {
        vl53l5cx_stop_ranging(&Dev);
        Debugger::printf("Stop ranging continuous\n");
        loop_count++;
    }

    if (loop_count > 20) {
        Debugger::printf("End of ULD demo\n");
    }
    */

} // loop
