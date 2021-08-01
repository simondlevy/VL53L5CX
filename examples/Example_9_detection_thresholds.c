/*******************************************************************************
* Copyright (c) 2020, STMicroelectronics - All Rights Reserved
*
* This file is part of the VL53L5CX Ultra Lite Driver and is dual licensed,
* either 'STMicroelectronics Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, the VL53L5CX Ultra Lite Driver may be distributed under the
* terms of 'BSD 3-clause "New" or "Revised" License', in which case the
* following provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*******************************************************************************/

/***********************************/
/* VL53L5CX ULD interrupt checkers */
/***********************************/
/*
* This example shows the possibility of VL53L5CX to program detection thresholds. It
* initializes the VL53L5CX ULD, create 2 thresholds per zone for a 4x4 resolution,
* and starts a ranging to capture 10 frames.

* In this example, we also suppose that the number of target per zone is
* set to 1 , and all output are enabled (see file platform.h).
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vl53l5cx_api.h"
#include "vl53l5cx_plugin_detection_thresholds.h"

// #define UNUSED(x) (void)(x)

/* This function needs to be filled by the customer. It allows knowing when 
 * the VL53L5CX interrupt is raised on GPIO1. This is the only way to use detection thresholds.
 */
/*
int WaitForL5Interrupt(VL53L5CX_Configuration * pDev) {

	//Add your implementation here ...
	UNUSED(pDev);

	return 0;
}
*/
extern int WaitForL5Interrupt(VL53L5CX_Configuration * pDev);
extern int IntCount;

int example9(void)
{

	/*********************************/
	/*   VL53L5CX ranging variables  */
	/*********************************/

	uint8_t 				status, loop, isAlive, isReady, i;
	VL53L5CX_Configuration 	Dev;			/* Sensor configuration */
	VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */

	
	/*********************************/
	/*      Customer platform        */
	/*********************************/

	/* Fill the platform structure with customer's implementation. For this
	* example, only the I2C address is used.
	*/
	Dev.platform.address = VL53L5CX_DEFAULT_I2C_ADDRESS;

	/* (Optional) Reset sensor toggling PINs (see platform, not in API) */
	//Reset_Sensor(&(Dev.platform));

	/* (Optional) Set a new I2C address if the wanted address is different
	* from the default one (filled with 0x20 for this example).
	*/
	//status = vl53l5cx_set_i2c_address(&Dev, 0x20);

	
	/*********************************/
	/*   Power on sensor and init    */
	/*********************************/

	/* (Optional) Check if there is a VL53L5CX sensor connected */
	status = vl53l5cx_is_alive(&Dev, &isAlive);
	if(!isAlive || status)
	{
		printf("VL53L5CX not detected at requested address\n");
		return status;
	}

	/* (Mandatory) Init VL53L5CX sensor */
	status = vl53l5cx_init(&Dev);
	if(status)
	{
		printf("VL53L5CX ULD Loading failed\n");
		return status;
	}

	printf("VL53L5CX ULD ready ! (Version : %s)\n",
			VL53L5CX_API_REVISION);
			
	/*********************************/
	/*  Program detection thresholds */
	/*********************************/

	/* In this example, we want 2 thresholds per zone for a 4x4 resolution */
	/* Create array of thresholds (size cannot be changed) */
	VL53L5CX_DetectionThresholds thresholds[VL53L5CX_NB_THRESHOLDS];

	/* Set all values to 0 */
	memset(&thresholds, 0, sizeof(thresholds));

	/* Add thresholds for all zones (16 zones in resolution 4x4, or 64 in 8x8) */
	for(i = 0; i < 16; i++){
		/* The first wanted thresholds is GREATER_THAN mode. Please note that the
		 * first one must always be set with a mathematic_operation
		 * VL53L5CX_OPERATION_NONE.
		 * For this example, the signal thresholds is set to 150 kcps/spads
		 * (the format is automatically updated inside driver)
		 */
		thresholds[2*i].zone_num = i;
		thresholds[2*i].measurement = VL53L5CX_SIGNAL_PER_SPAD_KCPS;
		thresholds[2*i].type = VL53L5CX_GREATER_THAN_MAX_CHECKER;
		thresholds[2*i].mathematic_operation = VL53L5CX_OPERATION_NONE;
		thresholds[2*i].param_low_thresh = 150;
		thresholds[2*i].param_high_thresh = 150;

		/* The second wanted checker is IN_WINDOW mode. We will set a
		 * mathematical thresholds VL53L5CX_OPERATION_OR, to add the previous
		 * checker to this one.
		 * For this example, distance thresholds are set between 200mm and
		 * 400mm (the format is automatically updated inside driver).
		 */
		thresholds[2*i+1].zone_num = i;
		thresholds[2*i+1].measurement = VL53L5CX_DISTANCE_MM;
		thresholds[2*i+1].type = VL53L5CX_IN_WINDOW;
		thresholds[2*i+1].mathematic_operation = VL53L5CX_OPERATION_OR;
		thresholds[2*i+1].param_low_thresh = 200;
		thresholds[2*i+1].param_high_thresh = 400;
	}

	/* The last thresholds must be clearly indicated. As we have 32
	 * checkers (16 zones x 2), the last one is the 31 */
	thresholds[31].zone_num = VL53L5CX_LAST_THRESHOLD | thresholds[31].zone_num;

	/* Send array of thresholds to the sensor */
	vl53l5cx_set_detection_thresholds(&Dev, thresholds);

	/* Enable detection thresholds */
	vl53l5cx_set_detection_thresholds_enable(&Dev, 1);

	/*********************************/
	/*         Ranging loop          */
	/*********************************/

	status = vl53l5cx_set_ranging_frequency_hz(&Dev, 10);

	IntCount = 0;
	status = vl53l5cx_start_ranging(&Dev);
	printf("Put an object between 200mm and 400mm to catch an interrupt\n");

	loop = 0;
	while(loop < 100)
	{
		/* Function WaitForL5Interrupt() does not exists, and must be
		 * implemented by user. It allows catching the interrupt raised on
		 * pin A3 (INT), when the checkers detect the programmed
		 * conditions.
		 */

		isReady = WaitForL5Interrupt(&Dev);
		if(isReady)
		{
			vl53l5cx_get_ranging_data(&Dev, &Results);

				/* As the sensor is set in 4x4 mode by default, we have a total
				 * of 16 zones to print. For this example, only the data of
				 * first zone are print */
			printf("Print data no : %3u\n", Dev.streamcount);
			for(i = 0; i < 16; i++)
			{
				printf("Zone : %3d, Status : %3u, Distance : %4d mm, Signal : %5lu kcps/SPADs\n",
					i,
					Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
					Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i],
					Results.signal_per_spad[VL53L5CX_NB_TARGET_PER_ZONE*i]);
			}
			printf("\n");
			loop++;
		}

		/* Wait a few ms to avoid too high polling (function in platform
		 * file, not in API) */
		WaitMs(&(Dev.platform), 5);
	}

	status = vl53l5cx_stop_ranging(&Dev);
	printf("End of ULD demo\n");
	return status;
}
