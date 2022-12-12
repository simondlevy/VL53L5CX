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

    public:

        typedef enum {

            RES_4X4_HZ_1 = 1,
            RES_4X4_HZ_2,
            RES_4X4_HZ_3,
            RES_4X4_HZ_4,
            RES_4X4_HZ_5,
            RES_4X4_HZ_6,
            RES_4X4_HZ_7,
            RES_4X4_HZ_8,
            RES_4X4_HZ_9,
            RES_4X4_HZ_10,
            RES_4X4_HZ_11,
            RES_4X4_HZ_12,
            RES_4X4_HZ_13,
            RES_4X4_HZ_14,
            RES_4X4_HZ_15,
            RES_4X4_HZ_16,
            RES_4X4_HZ_17,
            RES_4X4_HZ_18,
            RES_4X4_HZ_19,
            RES_4X4_HZ_20,
            RES_4X4_HZ_21,
            RES_4X4_HZ_22,
            RES_4X4_HZ_23,
            RES_4X4_HZ_24,
            RES_4X4_HZ_25,
            RES_4X4_HZ_26,
            RES_4X4_HZ_27,
            RES_4X4_HZ_28,
            RES_4X4_HZ_29,
            RES_4X4_HZ_30,
            RES_4X4_HZ_31,
            RES_4X4_HZ_32,
            RES_4X4_HZ_33,
            RES_4X4_HZ_34,
            RES_4X4_HZ_35,
            RES_4X4_HZ_36,
            RES_4X4_HZ_37,
            RES_4X4_HZ_38,
            RES_4X4_HZ_39,
            RES_4X4_HZ_40,
            RES_4X4_HZ_41,
            RES_4X4_HZ_42,
            RES_4X4_HZ_43,
            RES_4X4_HZ_44,
            RES_4X4_HZ_45,
            RES_4X4_HZ_46,
            RES_4X4_HZ_47,
            RES_4X4_HZ_48,
            RES_4X4_HZ_49,
            RES_4X4_HZ_50,
            RES_4X4_HZ_51,
            RES_4X4_HZ_52,
            RES_4X4_HZ_53,
            RES_4X4_HZ_54,
            RES_4X4_HZ_55,
            RES_4X4_HZ_56,
            RES_4X4_HZ_57,
            RES_4X4_HZ_58,
            RES_4X4_HZ_59,
            RES_4X4_HZ_60

        } res4X4_t;

        typedef enum {

            RES_8X8_HZ_1 = 1,
            RES_8X8_HZ_2,
            RES_8X8_HZ_3,
            RES_8X8_HZ_4,
            RES_8X8_HZ_5,
            RES_8X8_HZ_6,
            RES_8X8_HZ_7,
            RES_8X8_HZ_8,
            RES_8X8_HZ_9,
            RES_8X8_HZ_10,
            RES_8X8_HZ_11,
            RES_8X8_HZ_12,
            RES_8X8_HZ_13,
            RES_8X8_HZ_14,
            RES_8X8_HZ_15,

        } res8X8_t;

        VL53L5cx(
                const uint8_t lpnPin,
                const uint8_t integralTime,
                const res4X4_t resFreq,
                const uint8_t address=0x29)
            : VL53L5cx(lpnPin, integralTime, 16, (uint8_t)resFreq, address)
        {
        }

        VL53L5cx(
                const uint8_t lpnPin,
                const uint8_t integralTime,
                const res8X8_t resFreq,
                const uint8_t address=0x29)
            : VL53L5cx(lpnPin, integralTime, 64, (uint8_t)resFreq, address)
        {
        }

        void begin(void)
        {
            m_dev.platform.address = m_address;

            // Reset the sensor by toggling the LPN pin
            Reset_Sensor(m_lpnPin);

            //uint8_t status = vl53l5cx_set_i2c_address(&m_dev, m_address);

            // Check if there is a VL53L5CX sensor connected
            uint8_t isAlive = 0;

            uint8_t error = vl53l5cx_is_alive(&m_dev, &isAlive);

            checkStatus(error, "VL53L5CX could not write to device");

            if (!isAlive) {
                Debugger::reportForever("VL53L5CX not detected at address 0x%0X", m_address);
            }

            // (Mandatory) Init VL53L5CX sensor
            checkStatus(vl53l5cx_init(&m_dev), "VL53L5CX ULD Loading failed");

            Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n", VL53L5CX_API_REVISION);

            // Set resolution. WARNING : As others settings depend to this one, it must
            // come first.
            checkStatus(vl53l5cx_set_resolution(&m_dev, m_resolution),
                "vl53l5cx_set_resolution failed, status %u\n");

            // Select operating mode
            if (m_integralTime > 0) {

                checkStatus(
                        vl53l5cx_set_ranging_mode(&m_dev, VL53L5CX_RANGING_MODE_AUTONOMOUS),
                            "vl53l5cx_set_ranging_mode failed, status %u\n");

                // can set integration time in autonomous mode
                checkStatus(vl53l5cx_set_integration_time_ms(&m_dev, m_integralTime),
                            "vl53l5cx_set_integration_time_ms failed, status %u\n");
            }
            else { 
                // set continuous ranging mode, integration time is fixed in
                // continuous mode
                checkStatus(vl53l5cx_set_ranging_mode(&m_dev, VL53L5CX_RANGING_MODE_CONTINUOUS),
                        "vl53l5cx_set_ranging_mode failed, status %u\n");
            }

            // Select data rate 
            checkStatus(vl53l5cx_set_ranging_frequency_hz(&m_dev, m_frequency),
                    "vl53l5cx_set_ranging_frequency_hz failed, status %u\n");

            // Set target order to closest 
            checkStatus(vl53l5cx_set_target_order(&m_dev, VL53L5CX_TARGET_ORDER_CLOSEST),
                "vl53l5cx_set_target_order failed, status %u\n");

            // Get current integration time 
            uint32_t integration_time_ms = 0;
            checkStatus(vl53l5cx_get_integration_time_ms(&m_dev, &integration_time_ms),
                        "vl53l5cx_get_integration_time_ms failed, status %u\n");
            Debugger::printf(
                    "Current integration time is : %d ms\n", (int)integration_time_ms);

            /*
            // Put the VL53L5CX to sleep
            checkStatus(vl53l5cx_set_power_mode(&m_dev, VL53L5CX_POWER_MODE_SLEEP),
                "vl53l5cx_set_power_mode failed, status %u\n");
            Debugger::printf("VL53L5CX is now sleeping\n");

            // Restart
            checkStatus(vl53l5cx_set_power_mode(&m_dev, VL53L5CX_POWER_MODE_WAKEUP),
                "vl53l5cx_set_power_mode failed, status %u\n");
            Debugger::printf("VL53L5CX is now waking up\n");
            */

            // Start ranging 
            checkStatus(vl53l5cx_start_ranging(&m_dev), "start error = 0x%02X\n"); 

            uint8_t isReady = 0;

            // Clear the interrupt
            checkStatus(vl53l5cx_check_data_ready(&m_dev, &isReady), "check data ready: %u\n"); 
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
            return m_resolution;
         }

        uint8_t getTargetStatus(const uint8_t pixel)
        {
            return m_results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * pixel];
        }

        int16_t getDistanceMm(const uint8_t pixel)
        {
            return m_results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * pixel];
        }

        uint8_t getTargetDetectedCount(const uint8_t pixel)
        {
            return m_results.nb_target_detected[pixel];
        }

        uint8_t getAmbientPerSpad(const uint8_t pixel)
        {
            return m_results.ambient_per_spad[pixel];
        }

    private:

        VL53L5CX_Configuration m_dev;
        VL53L5CX_ResultsData m_results;

        uint8_t m_lpnPin;
        uint8_t m_address;
        uint8_t m_resolution;
        uint8_t m_frequency;
        uint8_t m_integralTime;

        VL53L5cx(
                const uint8_t lpnPin,
                const uint8_t integralTime,
                const uint8_t res,
                const uint8_t freq,
                const uint8_t address)
        {
            m_lpnPin = lpnPin;
            m_address = address;
            m_integralTime = integralTime;
            m_resolution = res;
            m_frequency = freq;
        }

        static void checkStatus(const uint8_t error, const char * fmt)
        {
            Debugger::checkStatus(error, fmt);
        }

}; // class VL53L5cx
