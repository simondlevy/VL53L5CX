/*
 *  VL53L5CX ULD basic example    
 *
 *  Copyright (c) 2021 Kris Winer and Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "NewVL53L5cx.h"
#include "Debugger.h"

static const uint8_t INT_PIN =  4;  // 8;

static VL53L5cx _sensor;

// Sum of integration time (1x for 4 x 4 and 4x for 8 x 8) must be 1 ms less
// than 1/freq, otherwise data rate decreased so integration time must be > 18
// ms at 4x4, 60 Hz, for example the smaller the integration time, the less
// power used, the more noise in the ranging data

static volatile bool gotnterrupt;

static void interruptHandler() {
    gotnterrupt = true;
}

void setup(void)
{
    // Start serial debugging
    Serial.begin(115200);
    delay(4000);
    Debugger::printf("Serial begun!\n");

    pinMode(INT_PIN, INPUT);     // VL53L5CX interrupt pin

    Wire.begin();                // Start I2C
    Wire.setClock(400000);       // Set I2C frequency at 400 kHz  
    delay(1000);

    Debugger::printf("starting\n\n");

    delay(1000);

    // Configure the data ready interrupt
    attachInterrupt(INT_PIN, interruptHandler, FALLING);

    _sensor.begin();

} // setup


void loop(void)
{
    if (gotnterrupt) {

        gotnterrupt = false;

        while (true) {

            uint8_t isReady = 0;

            uint8_t error = vl53l5cx_check_data_ready(&_sensor.Dev, &isReady);

            if (error !=0) {
                Debugger::printf("ready error = 0x%02X\n", error); 
            }

            if (isReady) {
                break;
            }

            delay(10);
        }

        // status = vl53l5cx_get_resolution(&_sensor.Dev, &resolution);
        vl53l5cx_get_ranging_data(&_sensor.Dev, &_sensor.Results);

        for (auto i=0; i<_sensor.pixels; i++) {

            // Print per zone results 
            Debugger::printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
                    i,
                    _sensor.Results.nb_target_detected[i],
                    _sensor.Results.ambient_per_spad[i]);

            // Print per target results 
            if (_sensor.Results.nb_target_detected[i] > 0) {
                Debugger::printf("Target status : %3u, Distance : %4d mm\n",
                        _sensor.Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * i],
                        _sensor.Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i]);
            }
            else {
                Debugger::printf("Target status : 255, Distance : No target\n");
            }
        }
        Debugger::printf("\n");

    } // end of VL53L5CX interrupt handling

} // loop


