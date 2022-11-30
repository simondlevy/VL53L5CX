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

    public:

        static const uint8_t NB_TARGET_PER_ZONE = VL53L5CX_NB_TARGET_PER_ZONE;

        typedef enum {

            RESOLUTION_4X4,
            RESOLUTION_8X8 

        } resolution_t;

        typedef enum {

            TARGET_ORDER_CLOSEST,
            TARGET_ORDER_STRONGEST

        } target_order_t;

        class XtalkCalibrationData {

            friend class VL53L5cx;

            protected:

                uint8_t data[VL53L5CX_XTALK_BUFFER_SIZE];
        };

        typedef struct {
            uint16_t kpcs_spads_min;
            uint16_t kpcs_spads_max;
            uint16_t distance_min;
            uint16_t distance_max;
        } detection_thresholds_t;

        VL53L5cx(
                uint8_t lpnPin,
                resolution_t resolution=RESOLUTION_4X4,
                target_order_t targetOrder=TARGET_ORDER_CLOSEST,
                uint8_t rangingFrequency=1);

        void begin(uint8_t address=0x29);

        void begin(detection_thresholds_t & thresholds, uint8_t address=0x29);

        bool isReady(void);

        void collectData(void);

        uint8_t getStreamCount(void);

        uint8_t getTargetStatus(uint8_t zone, uint8_t target=0);

        int16_t getDistance(uint8_t zone, uint8_t target=0);

        uint8_t getSignalPerSpad(uint8_t zone, uint8_t target=0);

        uint8_t getRangeSigma(uint8_t zone, uint8_t target=0);

        uint32_t getIntegrationTimeMsec(void);

        uint8_t getNbTargetDetected(uint8_t zone);

        uint8_t getAmbientPerSpad(uint8_t zone);

        uint8_t getNbSpadsEnabled(uint8_t zone);

        /**
        * Difference between min and max must never be >1500mm, and minimum never
        * be <400mm
        */
        void addMotionIndicator(uint16_t distanceMin=0, uint16_t distanceMax=0);

        /**
         * @brief This function starts the VL53L5CX sensor in order to calibrate Xtalk.
         * This calibration is recommended is user wants to use a coverglass.
         *
         * @param reflectancePercent : Target reflectance in percent. This
         * value is include between 1 and 99%. For a better efficiency, ST recommends a
         * 3% target reflectance.
         *
         * @param samples : Nb of samples used for calibration. A higher
         * number of samples means a higher accuracy, but it increases the calibration
         * time. Minimum is 1 and maximum is 16.
         *
         * @param distance: Target distance in mm. The minimum allowed
         * distance is 600mm, and maximum is 3000mm. The target must stay in Full FOV,
         * so short distance are easier for calibration.
         *
         */
        void calibrateXtalk(uint8_t reflectancePercent, uint8_t samples, uint16_t distance);

        void getXtalkCalibrationData(VL53L5cx::XtalkCalibrationData & data);

        void setXtalkCalibrationData(VL53L5cx::XtalkCalibrationData & data);

        void stop(void);

    protected:

        VL53L5CX_Configuration _dev = {};

        void init(uint8_t address);

        void start_ranging(void);

        static void checkStatus(uint8_t error, const char * fmt);

        uint8_t _ranging_frequency = 10;

    private:

        uint8_t _lpn_pin = 0;

        uint16_t _address = 0;

        VL53L5CX_ResultsData _results = {};

        resolution_t _resolution = RESOLUTION_8X8;

        target_order_t _target_order = TARGET_ORDER_CLOSEST;

        void check_ranging_frequency(resolution_t resolution,
                uint8_t maxval,
                const char *label);

        static void bozoFilter(bool cond, const char * msg);

        static void rangeFilter(uint16_t val, uint16_t minval, uint16_t maxval, const char * valname);

        static void make_detection_thresholds_array(
                detection_thresholds_t & values, 
                VL53L5CX_DetectionThresholds * array,
                resolution_t resolution);

}; // class VL53L5cx 

class VL53L5cxAutonomous : public VL53L5cx {

    private:

        uint32_t _integration_time_msec = 0;

    public:

        VL53L5cxAutonomous(
                uint8_t lpnPin,
                uint32_t integrationTimeMsec=20,
                resolution_t resolution=RESOLUTION_8X8,
                target_order_t targetOrder=TARGET_ORDER_CLOSEST);

        void begin(uint8_t address=0x29);

}; // class VL53L5cxAutonomous


