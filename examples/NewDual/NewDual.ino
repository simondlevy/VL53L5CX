/*
 *  VL53L5CX ULD basic example    
 *
 *  Copyright (c) 2021 Kris Winer and Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "VL53L5cx.h"
#include "Debugger.h"
#include "I2CScanner.h"
#include "st/vl53l5cx_api.h"

static const uint8_t LPN_PIN_0 = 17;
static const uint8_t INT_PIN_0 = 14;

static const uint8_t LPN_PIN_1 = 15;
static const uint8_t INT_PIN_1 = 16;

uint8_t  status, isAlive = 0, isReady, error, pixels;
uint32_t integration_time_ms;

static volatile bool VL53L5_intFlag_0 = false;
static volatile bool VL53L5_intFlag_1 = false;

static VL53L5CX_Configuration Dev_0 = {};  

static VL53L5CX_Configuration Dev_1 = {};  

// Configure VL53L5 measurement parameters
const uint8_t continuous_mode = 0;
const uint8_t autonomous_mode = 1;
const uint8_t VL53L5_mode = autonomous_mode; // either or

const uint8_t resolution_4x4 = 0;
const uint8_t resolution_8x8 = 1;
const uint8_t VL53L5_resolution = resolution_4x4; // either or

const uint8_t VL53L5_freq = 1;     // Min freq is 1 Hz max is 15 Hz (8 x 8) or 60 Hz (4 x 4)
const uint8_t VL53L5_intTime = 10; // in milliseconds, settable only when in autonomous mode, otherwise a no op

static void interruptHandler0()
{
    VL53L5_intFlag_0 = true;
}


static void interruptHandler1()
{
    VL53L5_intFlag_1 = true;
}

static void setupInterrupt(const uint8_t pin, void (*handler)(void))
{
    pinMode(pin, INPUT);      
    attachInterrupt(pin, handler, FALLING);
}

static void start(VL53L5CX_Configuration & Dev, const uint8_t id)
{
    uint8_t isAlive = 0;
    error = vl53l5cx_is_alive(&Dev, &isAlive);
    if (!isAlive || error) {
        Debugger::reportForever("VL53L5CX_%d not detected at requested address", id);
    }

    error = vl53l5cx_init(&Dev);
    if (error) {
        Debugger::reportForever("VL53L5CX_%d ULD Loading failed", id);
    }

    Debugger::printf("VL53L5CX_%d ULD ready ! (Version : %s)\n", id, VL53L5CX_API_REVISION);

    // Set resolution. WARNING : As others settings depend to this
    // one, it must come first.
    if (VL53L5_resolution == resolution_4x4){
        pixels = 16;
        status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_4X4);
        if (status) {
            Debugger::printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }
    else {
        pixels = 64;
        status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
        if (status) {
            Debugger::printf("vl53l5cx_set_resolution failed, status %u\n", status);
        }
    }

    // Select operating mode //
    if (VL53L5_mode == autonomous_mode) {
        // set autonomous ranging mode //
        status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
        if (status) {
            Debugger::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }

        // can set integration time in autonomous mode //
        status = vl53l5cx_set_integration_time_ms(&Dev, VL53L5_intTime); 
        if (status) {
            Debugger::printf("vl53l5cx_set_integration_time_ms failed, status %u\n", status);
        }
    }
    else 
    { 
        // set continuous ranging mode, integration time is fixed in continuous mode //
        status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_CONTINUOUS);
        if (status) {
            Debugger::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        }
    }

    // Select data rate //
    status = vl53l5cx_set_ranging_frequency_hz(&Dev, VL53L5_freq);
    if (status) {
        Debugger::printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
    }

    // Set target order to closest //
    status = vl53l5cx_set_target_order(&Dev, VL53L5CX_TARGET_ORDER_CLOSEST);
    if (status) {
        Debugger::printf("vl53l5cx_set_target_order failed, status %u\n", status);
    }

    // Get current integration time //
    status = vl53l5cx_get_integration_time_ms(&Dev, &integration_time_ms);
    if (status) {
        Debugger::printf("vl53l5cx_get_integration_time_ms failed, status %u\n", status);
    }

    Debugger::printf("Current integration time is : %d ms\n", integration_time_ms);


    // Clear the interrupt
    uint8_t isReady = 0;
    vl53l5cx_check_data_ready(&Dev, &isReady); 

    error = vl53l5cx_start_ranging(&Dev);
    if (error !=0) {
        Debugger::printf("VL53L5CX_%d start error = 0x%02X", id, error);
    }
}

static void checkInterrupt(VL53L5CX_Configuration & Dev, const uint8_t id, volatile bool & flag)
{
    if (flag) {

        flag = false;

        uint8_t isReady = 0;

        vl53l5cx_check_data_ready(&Dev, &isReady);

        if (isReady) {

            VL53L5CX_ResultsData Results = {};

            vl53l5cx_get_ranging_data(&Dev, &Results);

            // As the sensor is set in 4x4 mode by default, we have a total of
            // 16 zones to print. For this example, only the data of first zone
            // are printed 
            Debugger::printf("VL53L5CX_%d data no : %3u\n", id, Dev.streamcount);

            for (auto i=0; i<pixels; i++) {
                Debugger::printf(
                        "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debugger::printf("\n");
        }
    }
}

void setup(void)
{
    // Start serial debugging
    Serial.begin(115200);
    delay(4000);
    Serial.println("Serial begun!");

    Wire.begin();                    // Start I2C
    Wire.setClock(400000);           // Set I2C frequency at 400 kHz  
    delay(1000);

    pinMode(LPN_PIN_0, OUTPUT);      // VL53L5CX_0 LPN pin
    pinMode(LPN_PIN_1, OUTPUT);      // VL53L5CX_1 LPN pin

    digitalWrite(LPN_PIN_0, LOW);    // disable VL53L5CX_0

    digitalWrite(LPN_PIN_1, HIGH);   // enable VL53L5CX_1

    Dev_0.platform.address = 0x29;
    Dev_1.platform.address = 0x29;

    status = vl53l5cx_set_i2c_address(&Dev_1, 0x27<<1);
    Dev_1.platform.address = 0x27;
    digitalWrite(LPN_PIN_0, HIGH);   // enable VL53L5CX_0
    delay(100);

    start(Dev_0, 0);
    start(Dev_1, 1);

    Serial.println("Scan for I2C devices: should see 0x27, 0x29");
    I2CScanner::scan(Wire);           // should detect VL53L5CX_0 at 0x29 and VL53L5CX_1 at 0x27   
    delay(1000);

    // Start interrupt handling
    setupInterrupt(INT_PIN_0, interruptHandler0);
    setupInterrupt(INT_PIN_1, interruptHandler1);

} // setup


void loop(void)
{
    checkInterrupt(Dev_0, 0, VL53L5_intFlag_0);
    checkInterrupt(Dev_1, 1, VL53L5_intFlag_1);

} // loop



