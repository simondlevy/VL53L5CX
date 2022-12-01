/*
*  VL53L5CX ULD basic example    
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include <Wire.h>
#include "Debugger.hpp"
#include "VL53L5cx.h"

static const uint8_t LPN_PIN = 14;

static VL53L5cx sensor = VL53L5cx(LPN_PIN);

void setup(void)
{
    Serial.begin(115200);
    delay(4000);
    Debugger::printf("Starting sensor; may take a few seconds ...\n");

    // Start I^2C at 400kHz
    Wire.begin();
    Wire.setClock(400000);
    delay(100);

    // Start sensor
    sensor.begin();

} // setup

void loop(void)
{
    // Use polling function to know when a new measurement is ready.
    if (sensor.isReady()) {

        sensor.collectData();

        for(uint8_t i=0; i<16; i++) {

            Debugger::printf(
                    "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                    i,
                    sensor.getTargetStatus(i),
                    sensor.getDistance(i));
        }
        Debugger::printf("\n");
    }

    delay(5);
}
