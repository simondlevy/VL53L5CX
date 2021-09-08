/*
 * This example shows the possibility of VL53L5CX to visualize Xtalk data. It
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
#include "st/vl53l5cx_api.h"
#include "st/vl53l5cx_plugin_xtalk.h"

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

    uint8_t xtalk_data[VL53L5CX_XTALK_BUFFER_SIZE] = {};

    error = vl53l5cx_calibrate_xtalk(&Dev, 3, 4, 600);
    if (error) {
        Debugger::reportForever("vl53l5cx_calibrate_xtalk failed, status %u\n", error);
    }
    else {

        Debugger::printf("Xtalk calibration done\n");

        // Get Xtalk calibration data, in order to use them later
        vl53l5cx_get_caldata_xtalk(&Dev, xtalk_data);

        // Set Xtalk calibration data
        vl53l5cx_set_caldata_xtalk(&Dev, xtalk_data);
    }

    // (Optional) Visualize Xtalk grid and Xtalk shape
    union Block_header *bh_ptr;
    uint32_t xtalk_signal_kcps_grid[VL53L5CX_RESOLUTION_8X8];
    uint16_t xtalk_shape_bins[144];

    // Swap buffer
    SwapBuffer(xtalk_data, VL53L5CX_XTALK_BUFFER_SIZE);

    // Get data
    for(uint32_t i = 0; i < VL53L5CX_XTALK_BUFFER_SIZE; i = i + 4) {

        bh_ptr = (union Block_header *)&(xtalk_data[i]);

        if (bh_ptr->idx == 0xA128){
            Debugger::printf("Xtalk shape bins located at position %#06x\n", i);
            for (uint8_t j = 0; j < 144; j++){
                memcpy(&(xtalk_shape_bins[j]), &(xtalk_data[i + 4 + j * 2]), 2);
                Debugger::printf("xtalk_shape_bins[%d] = %u\n", j, xtalk_shape_bins[j]);
            }
        }
        if (bh_ptr->idx == 0x9FFC){
            Debugger::printf("Xtalk signal kcps located at position %#06x\n", i);
            for (uint32_t j = 0; j < VL53L5CX_RESOLUTION_8X8; j++){
                memcpy(&(xtalk_signal_kcps_grid[j]), &(xtalk_data[i + 4 + j * 4]), 4);
                xtalk_signal_kcps_grid[j] /= 2048;
                Debugger::printf("xtalk_signal_kcps_grid[%d] = %d\n", j, xtalk_signal_kcps_grid[j]);
            }
        }
    }

    // Re-Swap buffer (in case of re-using data later)
    SwapBuffer(xtalk_data, VL53L5CX_XTALK_BUFFER_SIZE);

    vl53l5cx_start_ranging(&Dev);

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

} // loop
