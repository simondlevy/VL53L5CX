/*
   VL53L5cx class library header

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "st/vl53l5cx_api.h"
#include "st/vl53l5cx_plugin_detection_thresholds.h"

#include <stdint.h>

class VL53L5cx {

    //private:
    public:

        VL53L5CX_Configuration Dev;
        VL53L5CX_ResultsData Results;

    public:

}; // class VL53L5cx


