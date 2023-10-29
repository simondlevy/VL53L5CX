/*
 *  VL53L5CX dual-sensor example    
 *
 *  Copyright (c) 2021 Kris Winer and Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include "vl53l5cx.hpp"
#include "debugger.hpp"
#include "i2cscanner.hpp"

static const uint8_t LPN_PIN_0 = 17;
static const uint8_t LPN_PIN_1 = 15;

// Set interrupt pins to 0 for polling
static const uint8_t INT_PIN_0 = 14;
static const uint8_t INT_PIN_1 = 16;

// Set to 0 for continuous mode
static const uint8_t INTEGRAL_TIME_MS = 10;

static const VL53L5CX::res4X4_t RESOLUTION = VL53L5CX::RES_4X4_HZ_1;

static VL53L5CX _sensor0(&Wire, LPN_PIN_0, INTEGRAL_TIME_MS, RESOLUTION);
static VL53L5CX _sensor1(&Wire, LPN_PIN_1, INTEGRAL_TIME_MS, RESOLUTION);

static volatile bool interruptFlag0 = false;
static void interruptHandler0()
{
    interruptFlag0 = true;
}

static volatile bool interruptFlag1 = false;
static void interruptHandler1()
{
    interruptFlag1 = true;
}

static void setupInterrupt(const uint8_t pin, void (*handler)(void))
{
    pinMode(pin, INPUT);      
    attachInterrupt(pin, handler, FALLING);
}

static void checkInterrupt(VL53L5CX & sensor, const uint8_t pin, const uint8_t id, volatile bool & flag)
{
    if (pin == 0 || flag) {

        flag = false;

        while (!sensor.dataIsReady()) {
            delay(10);
        }

        sensor.readData();

        Debugger::printf("Sensor %d -------------------------------------\n", id);

        for (auto i=0; i<sensor.getPixelCount(); i++) {

            // Print per zone results 
            Debugger::printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
                    i, sensor.getTargetDetectedCount(i), sensor.getAmbientPerSpad(i));

            // Print per target results 
            if (sensor.getTargetDetectedCount(i) > 0) {
                Debugger::printf("Target status : %3u, Distance : %4d mm\n",
                        sensor.getTargetStatus(i), sensor.getDistanceMm(i));
            }
            else {
                Debugger::printf("Target status : 255, Distance : No target\n");
            }
        }
        Debugger::printf("\n");
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

    // Disable sensor0 before starting sensor1 on a different address
    _sensor0.disable();

    _sensor1.begin(0x27);

    // Now we can start sensor 0
    _sensor0.begin();

    Serial.println("Scan for I2C devices: should see 0x27, 0x29");
    I2CScanner::scan(Wire);           // should detect VL53L5CX_0 at 0x29 and VL53L5CX_1 at 0x27   
    delay(1000);

    // Start interrupt handling
    setupInterrupt(INT_PIN_0, interruptHandler0);
    setupInterrupt(INT_PIN_1, interruptHandler1);

} // setup

void loop(void)
{
    checkInterrupt(_sensor0, INT_PIN_0, 0, interruptFlag0);
    checkInterrupt(_sensor1, INT_PIN_1, 1, interruptFlag1);

} // loop
