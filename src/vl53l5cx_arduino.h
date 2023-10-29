/*
   VL53L5CX Arduino class library header

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "vl53l5cx.hpp"

class VL53L5CX_Arduino : public VL53L5CX {

    public:

        VL53L5CX_Arduino(
                const uint8_t lpnPin,
                const uint8_t integralTime,
                const res4X4_t resFreq,
                TwoWire * twoWire=&Wire,
                const uint8_t address=0x29)
            : VL53L5CX((void *)twoWire, lpnPin, integralTime, 16, (uint8_t)resFreq, address)
        {
        }

        VL53L5CX_Arduino(
                const uint8_t lpnPin,
                const uint8_t integralTime,
                const res8X8_t resFreq,
                TwoWire * twoWire=&Wire,
                const uint8_t address=0x29)
            : VL53L5CX((void *)twoWire, lpnPin, integralTime, 64, (uint8_t)resFreq, address)
        {
        }

}; // class VL53L5CX_Arduino
