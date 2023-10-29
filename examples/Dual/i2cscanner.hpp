/*
 * Scans for connected I2C devices on a variety of Arduino-compatible microcontrollers
 *
 * Copyright (C) 2022 Simon D. Levy
 *
 * MIT License
 */

#include <Arduino.h>
#include <Wire.h>

#include "debugger.hpp"

class I2CScanner {

    public:

        static void scan(TwoWire & wire)
        {  
            Debugger::printf("Scanning...\n");

            uint8_t nDevices = 0;

            for (uint8_t address = 1; address < 127; address++ ) {

                wire.beginTransmission(address);
                uint8_t error = wire.endTransmission();

                if (error == 0) {
                    Debugger::printf("I2C device found at address 0x%02X\n", address);
                    nDevices++;
                }
                else if (error==4) {
                    Debugger::printf("Unknown error at address 0x%02X\n", address);
                }    
            }

            if (nDevices == 0) {
                Debugger::printf("No I2C devices found\n");
            }
            else {
                Debugger::printf("Done\n"); 
            }

            delay(1000);
        }

}; // class I2CScanner
