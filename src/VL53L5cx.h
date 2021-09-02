/*
   VL53L5cx class library header

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <stdint.h>
#include "vl53l5cx_api.h"

class VL53L5cx {

    public:

        typedef enum {

            RESOLUTION_4X4,
            RESOLUTION_8X8 

        } resolution_t;

        typedef enum {

            TARGET_ORDER_CLOSEST,
            TARGET_ORDER_STRONGEST

        } target_order_t;

        VL53L5cx(
                uint8_t lpnPin,
                uint8_t deviceAddress=0x29,
                resolution_t resolution=RESOLUTION_8X8,
                target_order_t targetOrder=TARGET_ORDER_CLOSEST);

        void begin(void);

        bool isReady(void);

        uint8_t getStreamCount(void);

        uint8_t getTargetStatus(uint8_t zone);

        uint8_t getDistance(uint8_t zone);

        void stop(void);

    private:

        uint8_t _lpn_pin = 0;

        VL53L5CX_Configuration _dev = {};

        VL53L5CX_ResultsData _results = {};

        resolution_t _resolution = RESOLUTION_8X8;

        target_order_t _target_order = TARGET_ORDER_CLOSEST;

}; // class VL53L5cx 
