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
                target_order_t targetOrder=TARGET_ORDER_CLOSEST,
                uint8_t rangingFrequency=10);

        void begin(void);

        bool isReady(void);

        uint8_t getStreamCount(void);

        uint8_t getTargetStatus(uint8_t zone);

        uint8_t getDistance(uint8_t zone);

        uint32_t getIntegrationTimeMsec(void);

        void stop(void);

    protected:

        VL53L5CX_Configuration _dev = {};

        void init(void);

        void start_ranging(void);

    private:

        uint8_t _lpn_pin = 0;

        VL53L5CX_ResultsData _results = {};

        resolution_t _resolution = RESOLUTION_8X8;

        target_order_t _target_order = TARGET_ORDER_CLOSEST;

        uint8_t _ranging_frequency = 10;

        void check_ranging_frequency(resolution_t resolution,
                                     uint8_t maxval,
                                     const char *label);

}; // class VL53L5cx 


class VL53L5cxAutonomous : public VL53L5cx {

    private:

        uint32_t _integration_time_msec = 0;

    public:

      VL53L5cxAutonomous(
                uint8_t lpnPin,
                uint32_t integrationTimeMsec=20,
                uint8_t deviceAddress=0x29,
                resolution_t resolution=RESOLUTION_8X8,
                target_order_t targetOrder=TARGET_ORDER_CLOSEST);

      void begin(void);

}; // VL53L5cxAutonomous
