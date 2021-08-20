/*
 *  VL53L5CX ULD basic example    
 *
 *  Copyright (c) 2021 Kris Winer, Simon D. Levy
 *
 *  MIT License
 */

#include "Wire.h"

#include "Debugger.hpp"
#include "vl53l5cx_api.h"

static const uint8_t LED_PIN  = 13;
static const uint8_t INT_PIN = 8;
static const uint8_t LPN_PIN = 3;

static volatile bool VL53L5_intFlag = false;

static VL53L5CX_Configuration Dev = {};  // Sensor configuration
static VL53L5CX_ResultsData Results = {};  // Results data from VL53L5CX

static void VL53L5_intHandler(){
    VL53L5_intFlag = true;
}

void setup(void)
{
    // Start serial debugging
    Serial.begin(115200);
    delay(4000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // start with led on, active HIGH

    pinMode(INT_PIN, INPUT); // VL53L5CX interrupt pin

    // Start I^2C
    Wire.begin();
    Wire.setClock(400000); // I2C frequency at 400 kHz  
    delay(1000);

    Debugger::printf("starting\n\n");

    delay(1000);

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

    digitalWrite(LED_PIN, LOW); // turn off led when initiation successfull

    /*********************************/
    /*        Set some params        */
    /*********************************/

    /* Set resolution in 8x8. WARNING : As others settings depend to this
     * one, it must be the first to use.
     */
    uint8_t status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
    if (status) {
        Debugger::printf("vl53l5cx_set_resolution failed, status %u\n", status);
    }

    /* Set ranging frequency to 1Hz.
     * Using 4x4, min frequency is 1Hz and max is 60Hz
     * Using 8x8, min frequency is 1Hz and max is 15Hz
     */
    status = vl53l5cx_set_ranging_frequency_hz(&Dev, 1);
    if (status) {
        Debugger::printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
    }

    /* Set target order to closest */
    status = vl53l5cx_set_target_order(&Dev, VL53L5CX_TARGET_ORDER_CLOSEST);
    if (status) {
        Debugger::printf("vl53l5cx_set_target_order failed, status %u\n", status);
    }

    /* Get current integration time */
    uint32_t integration_time_ms = 0;
    status = vl53l5cx_get_integration_time_ms(&Dev, &integration_time_ms);
    if (status) {
        Debugger::printf("vl53l5cx_get_integration_time_ms failed, status %u\n", status);
    }
    Debugger::printf("Current integration time is : %d ms\n", integration_time_ms);

    attachInterrupt(INT_PIN, VL53L5_intHandler, FALLING);

    error = vl53l5cx_start_ranging(&Dev);
    if (error !=0) {
        Debugger::printf("start error = 0x%02X", error);
    }

} // setup


void loop(void)
{
    if (VL53L5_intFlag) {

        VL53L5_intFlag = false;

        uint8_t isReady = 0;
        while (isReady == 0){
            uint8_t error = vl53l5cx_check_data_ready(&Dev, &isReady);
            if (error) {
                Debugger::printf("ready error = 0x%02X\n", error);
            }
            delay(10);
        }

        if (isReady) {

            vl53l5cx_get_ranging_data(&Dev, &Results);

            /* As the sensor is set in 8x8 mode, we have a total
             * of 64 zones to print. For this example, only the data of
             * first zone are print */
            Debugger::printf("Print data no : %3u\n", Dev.streamcount);
            for(uint8_t i = 0; i < 64; i++) {
                Debugger::printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
        }
    } // end of VL53L5CX interrupt handling

    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW);

    STM32.sleep();

} // loop


