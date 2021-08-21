/*
 *
 * This example shows the differences between ranging modes of VL53L5CX
 * (mode continuous and autonomous). For both modes, it initializes the VL53L5CX
 * ULD, set the mode, and starts a ranging to capture 10 frames.
 *
 * In this example, we also suppose that the number of target per zone is
 * set to 1 , and all output are enabled (see file platform.h).
 *
 *
 *  Copyright (c) 2021 Simon D. Levy
 *
 *  MIT License
*/ 


#include "Wire.h"

#include "Debugger.hpp"
#include "vl53l5cx_api.h"

void setup(void)
{
}

void loop(void)
{
}

/*
int example3(void)
{
    uint8_t 				status, loop, isAlive, isReady, i;
    VL53L5CX_Configuration 	Dev;			// Sensor configuration 
    VL53L5CX_ResultsData 	Results;		// Results data from VL53L5CX 


    // ******************************
    //      Customer platform        
    // ******************************

    // Fill the platform structure with customer's implementation. For this
    // example, only the I2C address is used.
    Dev.platform.address = VL53L5CX_DEFAULT_I2C_ADDRESS;

    // (Optional) Reset sensor toggling PINs (see platform, not in API) 
    //Reset_Sensor(&(Dev.platform));

    // (Optional) Set a new I2C address if the wanted address is different
    // from the default one (filled with 0x20 for this example).
    //status = vl53l5cx_set_i2c_address(&Dev, 0x20);


    // ******************************
    //   Power on sensor and init    
    // ******************************

    // (Optional) Check if there is a VL53L5CX sensor connected 
    status = vl53l5cx_is_alive(&Dev, &isAlive);
    if(!isAlive || status)
    {
        Debug::printf("VL53L5CX not detected at requested address\n");
        return status;
    }

    // (Mandatory) Init VL53L5CX sensor 
    status = vl53l5cx_init(&Dev);
    if(status)
    {
        Debug::printf("VL53L5CX ULD Loading failed\n");
        return status;
    }

    Debug::printf("VL53L5CX ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);


    // ******************************
    //  Set ranging mode autonomous  
    // ******************************

    status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
    if(status)
    {
        Debug::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        return status;
    }

    // Using autonomous mode, the integration time can be updated (not possible
    // using continuous)
    status = vl53l5cx_set_integration_time_ms(&Dev, 20);

    // Start a ranging session 
    status = vl53l5cx_start_ranging(&Dev);
    Debug::printf("Start ranging autonomous\n");

    loop = 0;
    while(loop < 10)
    {
        status = vl53l5cx_check_data_ready(&Dev, &isReady);
        if(isReady)
        {
            vl53l5cx_get_ranging_data(&Dev, &Results);

            // As the sensor is set in 4x4 mode by default, we have a total
            // of 16 zones to print. For this example, only the data of first zone are
            // printed
            Debug::printf("Print data no : %3u\n", Dev.streamcount);
            for(i = 0; i < 16; i++)
            {
                Debug::printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debug::printf("\n");
            loop++;
        }

        // Wait a few ms to avoid too high polling (function in platform
        // file, not in API)
        WaitMs(&(Dev.platform), 5);
    }

    status = vl53l5cx_stop_ranging(&Dev);
    Debug::printf("Stop ranging autonomous\n");


    // ******************************
    // Set ranging mode continuous   
    // ******************************

    // In continuous mode, the integration time cannot be programmed
    // (automatically set to maximum value)

    status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_CONTINUOUS);
    if(status)
    {
        Debug::printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        return status;
    }

    // Trying to update value below will have no impact on integration time 
    //status = vl53l5cx_set_integration_time_ms(&Dev, 20);

    // Start a ranging session 
    status = vl53l5cx_start_ranging(&Dev);
    Debug::printf("Start ranging continuous\n");

    loop = 0;
    while(loop < 10)
    {
        status = vl53l5cx_check_data_ready(&Dev, &isReady);
        if(isReady)
        {
            vl53l5cx_get_ranging_data(&Dev, &Results);

            // As the sensor is set in 4x4 mode by default, we have a total
            // of 16 zones to print 
            Debug::printf("Print data no : %3u\n", Dev.streamcount);
            for(i = 0; i < 16; i++)
            {
                Debug::printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                        i,
                        Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                        Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
            }
            Debug::printf("\n");
            loop++;
        }

        // Wait a few ms to avoid too high polling (function in platform
        // file, not in API)
        WaitMs(&(Dev.platform), 5);
    }

    status = vl53l5cx_stop_ranging(&Dev);
    Debug::printf("Stop ranging continuous\n");

    Debug::printf("End of ULD demo\n");
    return status;
}
*/
