/*
   VL53L5cx class library header

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "Debugger.h"

#include "st/vl53l5cx_api.h"
#include "st/vl53l5cx_plugin_detection_thresholds.h"

#include <stdint.h>

class VL53L5cx {

    private:

        static const uint8_t resolution_4x4 = 0;
        static const uint8_t resolution_8x8 = 1;

        static const uint8_t VL53L5_resolution = resolution_4x4; // either or

        // Min freq is 1 Hz max is 15 Hz (8 x 8) or 60 Hz (4 x 4)
        static const uint8_t VL53L5_freq = 1;     

        // Configure VL53L5 measurement parameters
        static const uint8_t continuous_mode = 0;
        static const uint8_t autonomous_mode = 1;
        static const uint8_t VL53L5_mode = autonomous_mode; // either or

        // Sum of integration time (1x for 4 x 4 and 4x for 8 x 8) must be 1 ms less
        // than 1/freq, otherwise data rate decreased so integration time must be > 18
        // ms at 4x4, 60 Hz, for example the smaller the integration time, the less
        // power used, the more noise in the ranging data

        // in milliseconds, settable only when in autonomous mode, otherwise a no op
        static const uint8_t VL53L5_intTime = 10; 

        VL53L5CX_Configuration m_dev;

        VL53L5CX_ResultsData m_results;

        uint8_t m_pixels;

        uint8_t m_lpnPin;

        uint8_t m_address;

        uint8_t m_resolution;

    public:

        typedef enum {

            RESOLUTION_4X4,
            RESOLUTION_8X8 

        } resolution_t;

        VL53L5cx(uint8_t lpnPin, resolution_t resolution=RESOLUTION_4X4, uint8_t address=0x29)
        {
            m_lpnPin = lpnPin;
            m_address = address;
            m_resolution = resolution == RESOLUTION_8X8 ? 1 : 0;
        }

        void begin(void)
        {
            m_dev.platform.address = m_address;

            // Reset the sensor by toggling the LPN pin
            Reset_Sensor(m_lpnPin);

            //status = vl53l5cx_set_i2c_address(&m_dev, m_address);

            // Check if there is a VL53L5CX sensor connected
            uint8_t isAlive = 0;
            uint8_t error = vl53l5cx_is_alive(&m_dev, &isAlive);
            if (!isAlive || error) {
                Debugger::reportForever("VL53L5CX not detected at requested address");
            }

            if (isAlive) {

                // (Mandatory) Init VL53L5CX sensor
                error = vl53l5cx_init(&m_dev);
                Debugger::printf("error = 0x%02X\n", error); 
                if (error) {
                    Debugger::reportForever("VL53L5CX ULD Loading failed");
                }

                Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n",
                        VL53L5CX_API_REVISION);
            }

            // Set resolution. WARNING : As others settings depend to this one, it must
            // come first.
            if (VL53L5_resolution == resolution_4x4) {
                m_pixels = 16;
                uint8_t status = vl53l5cx_set_resolution(&m_dev, VL53L5CX_RESOLUTION_4X4);
                if (status) {
                    Debugger::printf(
                            "vl53l5cx_set_resolution failed, status %u\n", status);
                }
            }
            else {
                m_pixels = 64;
                uint8_t status = vl53l5cx_set_resolution(&m_dev, VL53L5CX_RESOLUTION_8X8);
                if (status) {
                    Debugger::printf(
                            "vl53l5cx_set_resolution failed, status %u\n", status);
                }
            }

            // Select operating mode
            if (VL53L5_mode == autonomous_mode) {
                // set autonomous ranging mode
                uint8_t status =
                    vl53l5cx_set_ranging_mode(&m_dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
                if (status) {
                    Debugger::printf(
                            "vl53l5cx_set_ranging_mode failed, status %u\n", status);
                }

                // can set integration time in autonomous mode
                status = vl53l5cx_set_integration_time_ms(&m_dev, VL53L5_intTime); //  
                if (status) {
                    Debugger::printf(
                            "vl53l5cx_set_integration_time_ms failed, status %u\n",
                            status);
                }
            }
            else { 
                // set continuous ranging mode, integration time is fixed in
                // continuous mode
                uint8_t status =
                    vl53l5cx_set_ranging_mode(&m_dev, VL53L5CX_RANGING_MODE_CONTINUOUS);
                if (status) {
                    Debugger::printf("vl53l5cx_set_ranging_mode failed, status %u\n",
                            status);
                }
            }

            // Select data rate 
            uint8_t status = vl53l5cx_set_ranging_frequency_hz(&m_dev, VL53L5_freq);
            if (status) {
                Debugger::printf(
                        "vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
            }

            // Set target order to closest 
            status = vl53l5cx_set_target_order(&m_dev, VL53L5CX_TARGET_ORDER_CLOSEST);
            if (status) {
                Debugger::printf("vl53l5cx_set_target_order failed, status %u\n", status);
            }

            // Get current integration time 
            uint32_t integration_time_ms = 0;
            status = vl53l5cx_get_integration_time_ms(&m_dev, &integration_time_ms);
            if (status) {
                Debugger::printf(
                        "vl53l5cx_get_integration_time_ms failed, status %u\n", status);
            }
            Debugger::printf(
                    "Current integration time is : %d ms\n", (int)integration_time_ms);

            // *********
            // tailor functionality to decrease SRAM requirement, etc
            //  #define VL53L5CX_DISABLE_AMBIENT_PER_SPAD
            //  #define VL53L5CX_DISABLE_NB_SPADS_ENABLED
            //  #define VL53L5CX_DISABLE_SIGNAL_PER_SPAD
            //  #define VL53L5CX_DISABLE_RANGE_SIGMA_MM
            //  #define VL53L5CX_DISABLE_REFLECTANCE_PERCENT
            //  #define VL53L5CX_DISABLE_MOTION_INDICATOR
            // *********

            // Put the VL53L5CX to sleep
            status = vl53l5cx_set_power_mode(&m_dev, VL53L5CX_POWER_MODE_SLEEP);
            if (status) {
                Debugger::printf("vl53l5cx_set_power_mode failed, status %u\n", status);
            }
            Debugger::printf("VL53L5CX is now sleeping\n");

            // Restart
            status = vl53l5cx_set_power_mode(&m_dev, VL53L5CX_POWER_MODE_WAKEUP);
            if (status) {
                Debugger::printf("vl53l5cx_set_power_mode failed, status %u\n", status);
            }
            Debugger::printf("VL53L5CX is now waking up\n");

            // Start ranging 
            error = vl53l5cx_start_ranging(&m_dev);
            if (error !=0) {
                Debugger::printf("start error = 0x%02X\n", error); 
            }

            uint8_t isReady = 0;
            error =
                vl53l5cx_check_data_ready(&m_dev, &isReady); // clear the interrupt
        }

        bool dataIsReady(void)
        {
            uint8_t isReady = 0;

            uint8_t error = vl53l5cx_check_data_ready(&m_dev, &isReady);

            if (error !=0) {
                Debugger::printf("ready error = 0x%02X\n", error); 
            }

            return isReady != 0;
        }

        void readData(void)
        {
            // status = vl53l5cx_get_resolution(&m_dev, &resolution);
            vl53l5cx_get_ranging_data(&m_dev, &m_results);
        }

        uint8_t getPixelCount(void)
        {
            return m_pixels;
        }

        uint8_t getTargetStatus(uint8_t pixel)
        {
            return m_results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * pixel];
        }

        int16_t getDistanceMm(uint8_t pixel)
        {
            return m_results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * pixel];
        }

        uint8_t getTargetDetectedCount(uint8_t pixel)
        {
            return m_results.nb_target_detected[pixel];
        }

        uint8_t getAmbientPerSpad(uint8_t pixel)
        {
            return m_results.ambient_per_spad[pixel];
        }

}; // class VL53L5cx


