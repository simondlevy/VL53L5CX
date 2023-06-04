/*
   Cross-platform support

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <stdint.h>

#ifdef __linux__

static const uint8_t OUTPUT = 1;

extern void delay(const uint32_t msec);
extern void pinMode(const uint8_t pin, const uint8_t mode);
extern void digitalWrite(const uint8_t pin, const uint8_t value);
static const uint8_t HIGH = 1;
static const uint8_t LOW = 0;

static void outbuf(char * buf)
{
    puts(buf);
}

#else

#include <Arduino.h>

static void outbuf(char * buf)
{
    Serial.print(buf);
    Serial.flush();
}

#endif




