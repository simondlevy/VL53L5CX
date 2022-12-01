/*
 *  VL53L5CX ULD basic example    
 *
 *  Copyright (c) 2021 Kris Winer and Simon D. Levy
 *
 *  MIT License
 */

#include "Wire.h"

#include <VL53L5cx.h>
#include "Debugger.hpp"

static const uint8_t LED_PIN = 13;
static const uint8_t INT_PIN =  4;  // 8;
static const uint8_t LPN_PIN =  14; // 9;

static uint8_t  status, isAlive, isReady, error, pixels;
static uint32_t integration_time_ms;

static VL53L5CX_Configuration Dev = {};  // Sensor configuration
static VL53L5CX_ResultsData Results = {};  // Results data from VL53L5CX

// Configure VL53L5 measurement parameters
static const uint8_t continuous_mode = 0;
static const uint8_t autonomous_mode = 1;
static const uint8_t VL53L5_mode = autonomous_mode; // either or

static const uint8_t resolution_4x4 = 0;
static const uint8_t resolution_8x8 = 1;
static const uint8_t VL53L5_resolution = resolution_4x4; // either or

static const uint8_t VL53L5_freq = 1;     // Min freq is 1 Hz max is 15 Hz (8 x 8) or 60 Hz (4 x 4)
// Sum of integration time (1x for 4 x 4 and 4x for 8 x 8) must be 1 ms less than 1/freq, otherwise data rate decreased
// so integration time must be > 18 ms at 4x4, 60 Hz, for example
// the smaller the integration time, the less power used, the more noise in the ranging data
static const uint8_t VL53L5_intTime = 10; // in milliseconds, settable only when in autonomous mode, otherwise a no op

static volatile bool VL53L5_intFlag;

static void VL53L5_intHandler(){
    VL53L5_intFlag = true;
}

void setup(void)
{
    // Start serial debugging
    Serial.begin(115200);
    delay(4000);
    Serial.println("Serial begun!");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // start with led on, active HIGH

    pinMode(INT_PIN, INPUT);     // VL53L5CX interrupt pin

    Wire.begin();                // Start I2C
    Wire.setClock(400000);       // Set I2C frequency at 400 kHz  
    delay(1000);

    Debugger::printf("starting\n\n");

    delay(1000);

    // Fill the platform structure with customer's implementation. For this
    // example, only the I2C address is used.
    Dev.platform.address = 0x29;

    // Reset the sensor by toggling the LPN pin
    Reset_Sensor(LPN_PIN);

    /* (Optional) Set a new I2C address if the wanted address is different
     * from the default one (filled with 0x20 for this example).
     */
    //status = vl53l5cx_set_i2c_address(&Dev, 0x20);

    /*   Initialize the VL53L5CX   */
    // Check if there is a VL53L5CX sensor connected
    error = vl53l5cx_is_alive(&Dev, &isAlive);
    if(!isAlive || error) {
        Debugger::reportForever("VL53L5CX not detected at requested address");
    }

    if(isAlive) {
        // (Mandatory) Init VL53L5CX sensor
        error = vl53l5cx_init(&Dev);
        Serial.print("error = 0x"); Serial.println(error, HEX);
        if(error) {
            Debugger::reportForever("VL53L5CX ULD Loading failed");
        }

        Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n",
                VL53L5CX_API_REVISION);

        digitalWrite(LED_PIN, LOW); // turn off led when initiation successful
    }


    /*   Configure the VL53L5CX      */

    /* Set resolution. WARNING : As others settings depend to this
     * one, it must come first.
     */
    if(VL53L5_resolution == resolution_4x4){
        pixels = 16;
        status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_4X4);
        if(status) {
            printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }
    else {
        pixels = 64;
        status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
        if(status) {
            printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }

    /* Select operating mode */
    if(VL53L5_mode == autonomous_mode) {
        /* set autonomous ranging mode */
        status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
        if(status) {
            printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }

        /* can set integration time in autonomous mode */
        status = vl53l5cx_set_integration_time_ms(&Dev, VL53L5_intTime); //  
        if(status) {
            printf("vl53l5cx_set_integration_time_ms failed, status %u\n", status);
        }
    }
    else { 
        /* set continuous ranging mode, integration time is fixed in continuous mode */
        status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_CONTINUOUS);
        if(status) {
            printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }
    }

    /* Select data rate */
    status = vl53l5cx_set_ranging_frequency_hz(&Dev, VL53L5_freq);
    if(status) {
        printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
    }

    /* Set target order to closest */
    status = vl53l5cx_set_target_order(&Dev, VL53L5CX_TARGET_ORDER_CLOSEST);
    if(status) {
        printf("vl53l5cx_set_target_order failed, status %u\n", status);
    }

    /* Get current integration time */
    status = vl53l5cx_get_integration_time_ms(&Dev, &integration_time_ms);
    if(status) {
        printf("vl53l5cx_get_integration_time_ms failed, status %u\n", status);
    }
    printf("Current integration time is : %d ms\n", (int)integration_time_ms);


    // Configure the data ready interrupt
    attachInterrupt(INT_PIN, VL53L5_intHandler, FALLING);

    // *********
    // tailor functionality to decrease SRAM requirement, etc
    //  #define VL53L5CX_DISABLE_AMBIENT_PER_SPAD
    //  #define VL53L5CX_DISABLE_NB_SPADS_ENABLED
    //  #define VL53L5CX_DISABLE_SIGNAL_PER_SPAD
    //  #define VL53L5CX_DISABLE_RANGE_SIGMA_MM
    //  #define VL53L5CX_DISABLE_REFLECTANCE_PERCENT
    //  #define VL53L5CX_DISABLE_MOTION_INDICATOR
    // *********

    // Put the VL53L5CX to sleep
    status = vl53l5cx_set_power_mode(&Dev, VL53L5CX_POWER_MODE_SLEEP);
    if(status) {
        printf("vl53l5cx_set_power_mode failed, status %u\n", status);
    }
    printf("VL53L5CX is now sleeping\n");

    /* We wait 5 seconds, only for the example */
    printf("Waiting 5 seconds for the example...\n");
    WaitMs(&(Dev.platform), 5000);

    /* After 5 seconds, the sensor needs to be restarted */
    status = vl53l5cx_set_power_mode(&Dev, VL53L5CX_POWER_MODE_WAKEUP);
    if(status) {
        printf("vl53l5cx_set_power_mode failed, status %u\n", status);
    }
    printf("VL53L5CX is now waking up\n");


    /* Start ranging */
    error = vl53l5cx_start_ranging(&Dev);
    if(error !=0) {Serial.print("start error = "); Serial.println(error);}

    error = vl53l5cx_check_data_ready(&Dev, &isReady); // clear the interrupt

} // setup


void loop(void)
{
    if (VL53L5_intFlag) {

        VL53L5_intFlag = false;

        isReady = 0, error = 0;

        while(isReady == 0){
            error = vl53l5cx_check_data_ready(&Dev, &isReady);
            if(error !=0) {Serial.print("ready error = "); Serial.println(error);}
            delay(10);
        }

        if (isReady)
        {
            //      status = vl53l5cx_get_resolution(&Dev, &resolution);
            status = vl53l5cx_get_ranging_data(&Dev, &Results);

            for(int i = 0; i < pixels; i++){
                /* Print per zone results */
                printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
                        i,
                        Results.nb_target_detected[i],
                        Results.ambient_per_spad[i]);

                /* Print per target results */
                if(Results.nb_target_detected[i] > 0){
                    printf("Target status : %3u, Distance : %4d mm\n",
                            Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * i],
                            Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i]);
                }else{
                    printf("Target status : 255, Distance : No target\n");
                }
            }
            printf("\n");
        }

        digitalWrite(LED_PIN, HIGH); delay(10); digitalWrite(LED_PIN, LOW);

    } // end of VL53L5CX interrupt handling

} // loop


