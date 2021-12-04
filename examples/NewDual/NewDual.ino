/*
 *  VL53L5CX dual-sensor example with settable I^2C addresses
 *
 *  Copyright (c) 2021 Kris Winer and Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include <VL53L5cx.h>
#include <Debugger.hpp>
#include "st/vl53l5cx_api.h"

static const uint8_t LED_PIN = 13;

static const uint8_t INT_PIN_0 =  5;
static const uint8_t LPN_PIN_0 =  9;

static const uint8_t INT_PIN_1 =  8;
static const uint8_t LPN_PIN_1 =  4;

static uint8_t  status, isAlive = 0, isReady, error, pixels;

static volatile bool VL53L5_intFlag_0 = false;
static volatile bool VL53L5_intFlag_1 = false;

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

static void configure(VL53L5CX_Configuration * dev)
{
    //   Configure the VL53L5CX_0      //

    // Set resolution. WARNING : As others settings depend to this
    // one, it must come first.
    // 
    if(VL53L5_resolution == resolution_4x4){
        pixels = 16;
        status = vl53l5cx_set_resolution(dev, VL53L5CX_RESOLUTION_4X4);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }
    else
    {
        pixels = 64;
        status = vl53l5cx_set_resolution(dev, VL53L5CX_RESOLUTION_8X8);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }

    // Select operating mode //
    if(VL53L5_mode == autonomous_mode) {
        // set autonomous ranging mode //
        status = vl53l5cx_set_ranging_mode(dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }

        // can set integration time in autonomous mode //
        status = vl53l5cx_set_integration_time_ms(dev, VL53L5_intTime); //  
        if(status)
        {
            Debugger::printf("vl53l5cx_set_integration_time_ms failed, status %u\n", status);
        }
    }
    else 
    { 
        // set continuous ranging mode, integration time is fixed in continuous mode //
        status = vl53l5cx_set_ranging_mode(dev, VL53L5CX_RANGING_MODE_CONTINUOUS);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }
    }

    // Select data rate //
    status = vl53l5cx_set_ranging_frequency_hz(dev, VL53L5_freq);
    if(status)
    {
        Debugger::printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
    }

    // Set target order to closest //
    status = vl53l5cx_set_target_order(dev, VL53L5CX_TARGET_ORDER_CLOSEST);
    if(status)
    {
        Debugger::printf("vl53l5cx_set_target_order failed, status %u\n", status);
    }

    // Get current integration time //
    uint32_t integration_time_ms = 0;
    status = vl53l5cx_get_integration_time_ms(dev, &integration_time_ms);
    if(status)
    {
        Debugger::printf("vl53l5cx_get_integration_time_ms failed, status %u\n", status);
    }

    Debugger::printf("Current integration time is : %d ms\n", integration_time_ms);

} // configure


static VL53L5CX_Configuration Dev_0;  // Sensor configuration
static VL53L5CX_ResultsData Results_0;  // Results data from VL53L5CX_0

static VL53L5CX_Configuration Dev_1;  // Sensor configuration
static VL53L5CX_ResultsData Results_1;  // Results data from VL53L5CX_1

static void setupInterrupt(uint8_t pin, void (*handler)(void))
{
    pinMode(pin, INPUT);
    attachInterrupt(pin, handler, FALLING);
}

static void isr0()
{
    VL53L5_intFlag_0 = true;
}

static void isr1() 
{
    VL53L5_intFlag_1 = true;
}

void setup(void)
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // start with led on, active HIGH

    // Configure the data ready interrupts
    setupInterrupt(INT_PIN_0, isr0);
    setupInterrupt(INT_PIN_1, isr1);

    Wire.begin();                    // Start I2C
    Wire.setClock(400000);           // Set I2C frequency at 400 kHz  
    delay(1000);

    pinMode(LPN_PIN_0, OUTPUT);      // VL53L5CX_0 LPN pin
    digitalWrite(LPN_PIN_0, LOW);    // disable VL53L5CX_0

    digitalWrite(LPN_PIN_1, HIGH);   // enable VL53L5CX_0
    pinMode(LPN_PIN_1, OUTPUT);      // VL53L5CX_1 LPN pin

    // Fill the platform structure with customer's implementation. For this
    // example, only the I2C address is used.
    Dev_0.platform.address = 0x29;
    Dev_1.platform.address = 0x29;

    status = vl53l5cx_set_i2c_address(&Dev_1, 0x27<<1);
    Dev_1.platform.address = 0x27;
    digitalWrite(LPN_PIN_0, HIGH);   // enable VL53L5CX_0
    delay(100);

    // Make sure there is a VL53L5CX_0 sensor connected
    isAlive = 0;
    error = vl53l5cx_is_alive(&Dev_0, &isAlive);
    if(!isAlive || error) {
        Debugger::reportForever("VL53L5CX_0 not detected at requested address");
    }

    // Make sure there is a VL53L5CX_1 sensor connected
    isAlive = 0;
    error = vl53l5cx_is_alive(&Dev_1, &isAlive);
    if(!isAlive || error) {
        Debugger::reportForever("VL53L5CX_1 not detected at requested address");
    }

    // Init VL53L5CX_0 sensor
    error = vl53l5cx_init(&Dev_0);
    if(error) {
        Debugger::reportForever("VL53L5CX_0 ULD Loading failed");
    }

    Debugger::printf("VL53L5CX_0 ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

    // Init VL53L5CX sensor
    error = vl53l5cx_init(&Dev_1);
    if(error) {
        Debugger::reportForever("VL53L5CX_1 ULD Loading failed");
    }

    Debugger::printf("VL53L5CX_1 ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

    // change resolution, etc. before starting to range
    configure(&Dev_0);
    configureVL53L5_1();

    digitalWrite(LED_PIN, LOW); // turn off led when initiation successful

    error = vl53l5cx_start_ranging(&Dev_0);
    if(error !=0) {
        Debugger::printf("start error = 0x%02X", error);
    }

    error = vl53l5cx_start_ranging(&Dev_1);
    if(error !=0) {
        Debugger::printf("start error = 0x%02X", error);
    }


    error = vl53l5cx_check_data_ready(&Dev_0, &isReady); // clear the interrupt
    error = vl53l5cx_check_data_ready(&Dev_1, &isReady); // clear the interrupt

} /* end of setup */


void loop(void)
{
    if (VL53L5_intFlag_0) {
        VL53L5_intFlag_0 = false;

        isReady = 0;
        vl53l5cx_check_data_ready(&Dev_0, &isReady);

        if (isReady) {

            vl53l5cx_get_ranging_data(&Dev_0, &Results_0);

            // As the sensor is set in 4x4 mode by default, we have a total of
            // 16 zones to print. For this example, only the data of first zone
            // are printed 
            Debugger::printf("VL53L5CX_0 data no : %3u\n", Dev_0.streamcount);

            for(uint8_t i = 0; i < pixels; i++) {
                Debugger::printf(
                        "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results_0.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results_0.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
        }

    } // end of VL53L5CX_0 interrupt handling


    if (VL53L5_intFlag_1) {
        VL53L5_intFlag_1 = false;

        isReady = 0;
        vl53l5cx_check_data_ready(&Dev_1, &isReady);

        if (isReady) {

            vl53l5cx_get_ranging_data(&Dev_1, &Results_1);

            // As the sensor is set in 4x4 mode by default, we have a total of
            // 16 zones to print. For this example, only the data of first zone
            // are printed 
            Debugger::printf("VL53L5CX_1 data no : %3u\n", Dev_1.streamcount);

            for(uint8_t i = 0; i < pixels; i++) {
                Debugger::printf(
                        "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results_1.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results_1.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
        }

    } // end of VL53L5CX interrupt handling

    // Blinkety blink!
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW); 

} /* end of main loop */



void configureVL53L5_1()
{
    //   Configure the VL53L5CX_1      //

    // Set resolution. WARNING : As others settings depend to this
    // one, it must come first.
    // 
    if(VL53L5_resolution == resolution_4x4){
        pixels = 16;
        status = vl53l5cx_set_resolution(&Dev_1, VL53L5CX_RESOLUTION_4X4);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }
    else
    {
        pixels = 64;
        status = vl53l5cx_set_resolution(&Dev_1, VL53L5CX_RESOLUTION_8X8);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }

    // Select operating mode //
    if(VL53L5_mode == autonomous_mode) {
        // set autonomous ranging mode //
        status = vl53l5cx_set_ranging_mode(&Dev_1, VL53L5CX_RANGING_MODE_AUTONOMOUS);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }

        // can set integration time in autonomous mode //
        status = vl53l5cx_set_integration_time_ms(&Dev_1, VL53L5_intTime); //  
        if(status)
        {
            Debugger::printf("vl53l5cx_set_integration_time_ms failed, status %u\n", status);
        }
    }
    else 
    { 
        // set continuous ranging mode, integration time is fixed in continuous mode //
        status = vl53l5cx_set_ranging_mode(&Dev_1, VL53L5CX_RANGING_MODE_CONTINUOUS);
        if(status)
        {
            Debugger::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }
    }

    // Select data rate //
    status = vl53l5cx_set_ranging_frequency_hz(&Dev_1, VL53L5_freq);
    if(status)
    {
        Debugger::printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
    }

    // Set target order to closest //
    status = vl53l5cx_set_target_order(&Dev_1, VL53L5CX_TARGET_ORDER_CLOSEST);
    if(status)
    {
        Debugger::printf("vl53l5cx_set_target_order failed, status %u\n", status);
    }

    // Get current integration time //
    uint32_t integration_time_ms = 0;
    status = vl53l5cx_get_integration_time_ms(&Dev_1, &integration_time_ms);
    if(status)
    {
        Debugger::printf("vl53l5cx_get_integration_time_ms failed, status %u\n", status);
    }

    Debugger::printf("Current integration time is : %d ms\n", integration_time_ms);

}
