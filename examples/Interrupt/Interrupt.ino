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

static const uint8_t INTERRUPT_PIN =  4;

static VL53L5cx _sensor;

static volatile bool gotInterrupt;

static void interruptHandler() 
{
    gotInterrupt = true;
}

void setup(void)
{
    Serial.begin(115200);
    delay(4000);
    Debugger::printf("Serial begun!\n");

    pinMode(INTERRUPT_PIN, INPUT);     // VL53L5CX interrupt pin

    Wire.begin();                // Start I2C
    Wire.setClock(400000);       // Set I2C frequency at 400 kHz  
    delay(1000);

    Debugger::printf("starting\n\n");

    delay(1000);

    attachInterrupt(INTERRUPT_PIN, interruptHandler, FALLING);

    _sensor.begin();
}

void loop(void)
{
    if (gotInterrupt) {

        gotInterrupt = false;

        while (!_sensor.dataIsReady()) {
            delay(10);
        }

        _sensor.readData();

        for (auto i=0; i<_sensor.pixelCount(); i++) {

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
    } 
}


