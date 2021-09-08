/*
 * This example shows how to use the motion indicator with detection threshold.
 * This kind of configuration might be used for user detection applications.
 * To use this example, user needs to be sure that macro
 * VL53L5CX_DISABLE_MOTION_INDICATOR is NOT enabled (see file platform.h).
 *
 *  Copyright (c) 2021 Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "Debugger.hpp"

#include "st/vl53l5cx_api.h"
#include "st/vl53l5cx_plugin_motion_indicator.h"
#include "st/vl53l5cx_plugin_detection_thresholds.h"

static const uint8_t INT_PIN = 8;
static const uint8_t LPN_PIN = 5;

static VL53L5CX_Configuration Dev;                   // Sensor configuration
static VL53L5CX_ResultsData Results;                 // Results data from VL53L5CX
static VL53L5CX_Motion_Configuration motion_config;  // Motion configuration

static volatile bool VL53L5_intFlag;
static void VL53L5_intHandler(void)
{
    VL53L5_intFlag = true;
}

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

    // Create motion indicator with resolution 8x8
    error = vl53l5cx_motion_indicator_init(&Dev, &motion_config, VL53L5CX_RESOLUTION_8X8);
    if (error) {
        Debugger::printf("Motion indicator init failed with error : %u\n", error);
    }

    // (Optional) Change the min and max distance used to detect motions. The
    // difference between min and max must never be >1500mm, and minimum never be <400mm,
    // otherwise the function below returns error 127 */
    error = vl53l5cx_motion_indicator_set_distance_motion(&Dev, &motion_config, 1000, 2000);
    if (error) {
        Debugger::printf("Motion indicator set distance motion failed with error : %u\n", error);
    }

    // If user want to change the resolution, he also needs to update the motion indicator resolution
    // error = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_4X4);
    // error = vl53l5cx_motion_indicator_set_resolution(&Dev, &motion_config, VL53L5CX_RESOLUTION_4X4);


    // Set the device in AUTONOMOUS and set a small integration time to reduce power consumption
    error = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
    error = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
    error = vl53l5cx_set_ranging_frequency_hz(&Dev, 2);
    error = vl53l5cx_set_integration_time_ms(&Dev, 10);

    // In this example, we want 1 threshold per zone for a 8x8 resolution
    // Create array of thresholds (size cannot be changed)
    VL53L5CX_DetectionThresholds thresholds[VL53L5CX_NB_THRESHOLDS];

    // Set all values to 0
    memset(&thresholds, 0, sizeof(thresholds));

    // Add thresholds for all zones (64 zones in resolution 4x4, or 64 in 8x8)
    for (uint8_t i = 0; i < 64; i++){

        thresholds[i].zone_num = i;
        thresholds[i].measurement = VL53L5CX_MOTION_INDICATOR;
        thresholds[i].type = VL53L5CX_GREATER_THAN_MAX_CHECKER;
        thresholds[i].mathematic_operation = VL53L5CX_OPERATION_NONE;

        // The value 44 is given as example. All motion above 44 will be considered as a movement
        thresholds[i].param_low_thresh = 44;
        thresholds[i].param_high_thresh = 44;
    }

    // The last thresholds must be clearly indicated. As we have 64
    // checkers, the last one is the 63
    thresholds[63].zone_num = VL53L5CX_LAST_THRESHOLD | thresholds[63].zone_num;

    // Send array of thresholds to the sensor
    vl53l5cx_set_detection_thresholds(&Dev, thresholds);

    // Enable detection thresholds
    vl53l5cx_set_detection_thresholds_enable(&Dev, 1);

    // Set up interrupt
    pinMode(INT_PIN, INPUT);
    attachInterrupt(INT_PIN, VL53L5_intHandler, FALLING);

    // Start ranging
    vl53l5cx_start_ranging(&Dev);

} // setup

void loop(void)
{
    static uint8_t loop_count;

    if (loop_count < 10) {

        if (VL53L5_intFlag) {

            VL53L5_intFlag = false;

            vl53l5cx_get_ranging_data(&Dev, &Results);

            // As the sensor is set in 8x8 mode by default, we have a total of
            // 64 zones to print. For this example, only the data of first zone
            // are printed.
            for (uint8_t i = 0; i < 64; i++) {

                if (Results.motion_indicator.motion[motion_config.map_id[i]] >= 44) {
                    Debugger::printf(" Movement detected in this zone : %3d !\n", i);
                }
            }
            Debugger::printf("\n");
            loop_count++;
        }
    }

    else if (loop_count == 10) {
        vl53l5cx_stop_ranging(&Dev);
        loop_count++;
    }
    else {
        Debugger::printf("End of ULD demo\n");
        delay(500);
    }
}
