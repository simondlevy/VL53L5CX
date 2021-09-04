#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    uint16_t address;

} VL53L5CX_Platform;

static const uint8_t VL53L5CX_NB_TARGET_PER_ZONE = 1; 

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
