#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    uint16_t address;
    void * device;

} VL53L5CX_Platform;

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


