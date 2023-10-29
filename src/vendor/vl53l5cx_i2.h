#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    uint16_t address;
    void * device;

} VL53L5CX_Platform;

uint8_t VL53L1CX_ReadMulti(VL53L5CX_Platform *p_platform, uint16_t rgstr,
        uint8_t *data, uint32_t count);

uint8_t VL53L1CX_WriteMulti(VL53L5CX_Platform *p_platform, uint16_t rgstr,
        uint8_t *data, uint32_t count);


