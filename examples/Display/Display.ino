/*
*  VL53L5CX 8x8 display example.  Sends 64-byte messaages over serial to be
*  displayed by a program (e.g, OpenCV) on the host computer.
*
*  Copyright (c) 2021 Simon D. Levy
*
*  MIT License
*/

#include <Wire.h>
#include "vl53l5cx_arduino.h"

// Set to 0 for polling
static const uint8_t INT_PIN = 4;

static const uint8_t LPN_PIN =  14;

// Set to 0 for continuous mode
static const uint8_t INTEGRAL_TIME_MS = 10;

static VL53L5CX _sensor(&Wire, LPN_PIN, INTEGRAL_TIME_MS, VL53L5CX::RES_8X8_HZ_1);

static volatile bool _gotInterrupt;

static void interruptHandler() 
{
    _gotInterrupt = true;
}


void setup(void)
{
    Serial.begin(115200);

    pinMode(INT_PIN, INPUT);     

    Wire.begin();                
    Wire.setClock(400000);      
    delay(1000);

    delay(1000);

    if (INT_PIN > 0) {
        attachInterrupt(INT_PIN, interruptHandler, FALLING);
    }

    _sensor.begin();
}

void loop(void)
{
    if (INT_PIN == 0 || _gotInterrupt) {

        _gotInterrupt = false;

        while (!_sensor.dataIsReady()) {
            delay(10);
        }

        _sensor.readData();

        for (uint8_t i=0; i<64; i++) {

            Serial.write(_sensor.getDistanceMm(i));
        }

        Serial.write(0xFF); // sentinel byte
    }
}
