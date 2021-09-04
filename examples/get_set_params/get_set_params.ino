/*
 *  VL53L5CX ULD example with parameter-setting and interrupts
 *
 *  Copyright (c) 2021 Kris Winer, Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>
#include "Debugger.hpp"
#include "VL53L5cx.h"

static const uint8_t LED_PIN = 13;
static const uint8_t INT_PIN = 8;
static const uint8_t LPN_PIN = 5;

static VL53L5cx sensor = VL53L5cx(5,     // LPN pin
                                  0x29,  // device address
                                  VL53L5cx::RESOLUTION_8X8, 
                                  VL53L5cx::TARGET_ORDER_CLOSEST,
                                  1);    // ranging frequency 

// Interrupt service routine
static volatile bool VL53L5_intFlag;
static void VL53L5_intHandler(void)
{
    VL53L5_intFlag = true;
}

void setup(void)
{
    // Start serial debugging
    Serial.begin(115200);

    // Turn LED on
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); 

    // Start I^2C
    Wire.begin();
    Wire.setClock(400000); // I2C frequency at 400 kHz  
    delay(1000);

    Debugger::printf("starting\n\n");

    delay(1000);

    // Set up interrupt
    pinMode(INT_PIN, INPUT); 
    attachInterrupt(INT_PIN, VL53L5_intHandler, FALLING);

    // Start the sensor
    sensor.begin();

    Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

    // Get current integration time
    Debugger::printf("Current integration time is : %d ms\n", sensor.getIntegrationTimeMsec());

    // Turn off led when initiation successfull
    digitalWrite(LED_PIN, LOW); 

    delay(3000);

} // setup


void loop(void)
{
    if (VL53L5_intFlag) {

        VL53L5_intFlag = false;

        // Use polling function to know when a new measurement is ready.
        if (sensor.isReady()) {

            // As the sensor is set in 8x8 mode, we have a total
            // of 64 zones to print. For this example, only the data of
            // first zone are print 
            Debugger::printf("Print data no : %3u\n", sensor.getStreamCount());
            for(uint8_t i = 0; i < 64; i++) {
                Debugger::printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        sensor.getTargetStatus(i),
                        sensor.getDistance(i));
            }
            Debugger::printf("\n");

        }

    } // end of VL53L5CX interrupt handling

    // Flash the LED
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW);

} // loop


