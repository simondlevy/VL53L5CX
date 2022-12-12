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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    uint16_t address;

} VL53L5CX_Platform;

static const uint8_t VL53L5CX_NB_TARGET_PER_ZONE = 1; 

void setI2CDevice(void * dev);

uint8_t RdByte(
  VL53L5CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t *p_value);

uint8_t WrByte(
  VL53L5CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t value);

uint8_t RdMulti(
  VL53L5CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t *p_values,
  uint32_t size);

uint8_t WrMulti(
  VL53L5CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t *p_values,
  uint32_t size);

uint8_t Reset_Sensor(uint8_t lpn_pin);

void SwapBuffer(
  uint8_t   *buffer,
  uint16_t     size);

uint8_t WaitMs(
  VL53L5CX_Platform *p_platform,
  uint32_t TimeMs);
