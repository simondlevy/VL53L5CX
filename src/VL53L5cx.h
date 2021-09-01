/*
   VL53L5cx class library header

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <stdint.h>

class VL53L5cx {

    private:

        uint8_t _lpn_pin = 0;

    public:

        VL53L5cx(uint8_t lpnPin);

}; // class VL53L5cx 
