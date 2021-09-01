/*
   VL53L5cx class library implementation

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include "VL53L5cx.h"

VL53L5cx::VL53L5cx(uint8_t lpnPin)
{
    _lpn_pin = lpnPin;
}
