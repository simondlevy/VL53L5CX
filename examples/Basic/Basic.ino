/*
 *  VL53L5CX ULD basic example    
 *
 *  Copyright (c) 2022 Kris Winer and Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "VL53L5cx.h"
#include "Debugger.h"

static const uint8_t LPN_PIN =  14;
static const uint8_t INT_PIN =  4;

// Set to 0 for continuous mode
static const uint8_t INTEGRAL_TIME_MS = 10;

static VL53L5cx _sensor(LPN_PIN, INTEGRAL_TIME_MS, VL53L5cx::RES_4X4_HZ_1);

static volatile bool _gotInterrupt;

static void interruptHandler() 
{
    _gotInterrupt = true;
}

void setup(void)
{
    Serial.begin(115200);
    delay(4000);
    Debugger::printf("Serial begun!\n");

    pinMode(INT_PIN, INPUT);     

    Wire.begin();                
    Wire.setClock(400000);      
    delay(1000);

    Debugger::printf("starting\n\n");

    delay(1000);

    attachInterrupt(INT_PIN, interruptHandler, FALLING);

    _sensor.begin();
}

void loop(void)
{
    if (_gotInterrupt) {

        _gotInterrupt = false;

        while (!_sensor.dataIsReady()) {
            delay(10);
        }

        _sensor.readData();

        for (auto i=0; i<_sensor.getPixelCount(); i++) {

            // Print per zone results 
            Debugger::printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
                    i, _sensor.getTargetDetectedCount(i), _sensor.getAmbientPerSpad(i));

            // Print per target results 
            if (_sensor.getTargetDetectedCount(i) > 0) {
                Debugger::printf("Target status : %3u, Distance : %4d mm\n",
                        _sensor.getTargetStatus(i), _sensor.getDistanceMm(i));
            }
            else {
                Debugger::printf("Target status : 255, Distance : No target\n");
            }
        }
        Debugger::printf("\n");
    } 
}


