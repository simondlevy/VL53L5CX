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

static volatile bool VL53L5_intFlag_0 = false;
static volatile bool VL53L5_intFlag_1 = false;

static const uint8_t VL53L5_freq = 1;

// Min freq is 1 Hz max is 15 Hz (8 x 8) or 60 Hz (4 x 4) Sum of integration
// time (1x for 4 x 4 and 4x for 8 x 8) must be 1 ms less than 1/freq,
// otherwise data rate decreased so integration time must be > 18 ms at 4x4, 60
// Hz, for example the smaller the integration time, the less power used, the
// more noise in the ranging data

// time in milliseconds, settable only when in autonomous mode, otherwise a no op
static const uint8_t VL53L5_intTime = 10;

static void init(uint8_t id, VL53L5CX_Configuration * dev)
{
    uint8_t error = vl53l5cx_set_resolution(dev, VL53L5CX_RESOLUTION_4X4);
    if(error) {
        Debugger::printf("vl53l5cx_set_resolution failed, error %u\n", error);
    }

    // set autonomous ranging mode //
    error = vl53l5cx_set_ranging_mode(dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
    if(error) {
        Debugger::printf("vl53l5cx_set_ranging_mode failed, error %u\n", error);
    }

    // can set integration time in autonomous mode //
    error = vl53l5cx_set_integration_time_ms(dev, VL53L5_intTime); //  
    if(error) {
        Debugger::printf("vl53l5cx_set_integration_time_ms failed, error %u\n", error);
    }

    // Select data rate //
    error = vl53l5cx_set_ranging_frequency_hz(dev, VL53L5_freq);
    if(error) {
        Debugger::printf("vl53l5cx_set_ranging_frequency_hz failed, error %u\n", error);
    }

    // Set target order to closest //
    error = vl53l5cx_set_target_order(dev, VL53L5CX_TARGET_ORDER_CLOSEST);
    if(error) {
        Debugger::printf("vl53l5cx_set_target_order failed, error %u\n", error);
    }

    // Get current integration time //
    uint32_t integration_time_ms = 0;
    error = vl53l5cx_get_integration_time_ms(dev, &integration_time_ms);
    if(error) {
        Debugger::printf("vl53l5cx_get_integration_time_ms failed, error %u\n", error);
    }

    Debugger::printf("Current integration time is : %d ms\n", integration_time_ms);

    error = vl53l5cx_start_ranging(dev);
    if(error !=0) {
        Debugger::printf("start error = 0x%02X", error);
    }

    uint8_t isReady = 0;
    vl53l5cx_check_data_ready(dev, &isReady); // clear the interrupt

} // init


static VL53L5CX_Configuration Dev_0;  // Sensor configuration
static VL53L5CX_Configuration Dev_1;  // Sensor configuration

static void setupInterrupt(uint8_t pin, void (*handler)(void))
{
    pinMode(pin, INPUT);
    attachInterrupt(pin, handler, FALLING);
}

static void checkAndReport(uint8_t id, volatile bool & flag, VL53L5CX_Configuration * dev)
{
    if (flag) {

        flag = false;

        uint8_t isReady = 0;

        vl53l5cx_check_data_ready(dev, &isReady);

        if (isReady) {

            VL53L5CX_ResultsData results = {}; 

            vl53l5cx_get_ranging_data(dev, &results);

            // As the sensor is set in 4x4 mode by default, we have a total of
            // 16 zones to print. For this example, only the data of first zone
            // are printed 
            Debugger::printf("VL53L5CX_%d data no : %3u\n", id, dev->streamcount);

            for(uint8_t i=0; i<16; i++) {

                Debugger::printf(
                        "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
        }
    }
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

    pinMode(LPN_PIN_1, OUTPUT);      // VL53L5CX_1 LPN pin
    digitalWrite(LPN_PIN_1, HIGH);   // enable VL53L5CX_1

    // Fill the platform structure with customer's implementation. For this
    // example, only the I2C address is used.
    Dev_0.platform.address = 0x29;
    Dev_1.platform.address = 0x29;

    vl53l5cx_set_i2c_address(&Dev_1, 0x27<<1);
    Dev_1.platform.address = 0x27;
    digitalWrite(LPN_PIN_0, HIGH);   // enable VL53L5CX_0
    delay(100);

    // Make sure there is a VL53L5CX_0 sensor connected
    uint8_t isAlive = 0;
    uint8_t error = vl53l5cx_is_alive(&Dev_0, &isAlive);
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
    init(0, &Dev_0);
    init(1, &Dev_1);

    digitalWrite(LED_PIN, LOW); // turn off led when initiation successful

} // setup

void loop(void)
{
    checkAndReport(0, VL53L5_intFlag_0, &Dev_0);
    checkAndReport(1, VL53L5_intFlag_1, &Dev_1);

    // Blinkety blink!
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW); 
} 
