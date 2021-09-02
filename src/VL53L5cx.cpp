/*
   VL53L5cx class library implementation

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include "VL53L5cx.h"
#include "Debugger.hpp"

VL53L5cx::VL53L5cx(
        uint8_t lpnPin,
        uint8_t deviceAddress,
        resolution_t resolution,
        target_order_t targetOrder)
{
    _lpn_pin = lpnPin;

    _dev.platform.address = deviceAddress;

    _resolution = resolution;
    _target_order = targetOrder;
}
        
void VL53L5cx::begin(void)
{
    // Reset the sensor by toggling the LPN pin
    Reset_Sensor(_lpn_pin);

    // Make sure there is a VL53L5CX sensor connected
    uint8_t isAlive = 0;
    uint8_t error = vl53l5cx_is_alive(&_dev, &isAlive);
    if(!isAlive || error) {
        Debugger::reportForever("VL53L5CX not detected at requested address");
    }

    // Init VL53L5CX sensor
    error = vl53l5cx_init(&_dev);
    if(error) {
        Debugger::reportForever("VL53L5CX ULD Loading failed");
    }

    // Set resolution
    vl53l5cx_set_resolution(&_dev,
            _resolution == RESOLUTION_4X4 ?
            VL53L5CX_RESOLUTION_4X4 :
            VL53L5CX_RESOLUTION_8X8);

    // Set target order
    vl53l5cx_set_target_order(&_dev,
            _target_order == TARGET_ORDER_STRONGEST ?
            VL53L5CX_TARGET_ORDER_STRONGEST :
            VL53L5CX_TARGET_ORDER_CLOSEST);

    // Start ranging
    error = vl53l5cx_start_ranging(&_dev);
    if(error !=0) {
        Debugger::reportForever("start error = 0x%02X", error);
    }

} // begin

bool VL53L5cx::isReady(void)
{
    uint8_t is_ready = false;

    vl53l5cx_check_data_ready(&_dev, &is_ready);

    if (is_ready) {
        vl53l5cx_get_ranging_data(&_dev, &_results);
        return true;
    }

    return false;
}

uint8_t VL53L5cx::getStreamCount(void)
{
    return _dev.streamcount;
}

uint8_t VL53L5cx::getTargetStatus(uint8_t zone)
{
    return _results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * zone];
}

uint8_t VL53L5cx::getDistance(uint8_t zone)
{
    return _results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * zone];
}

void VL53L5cx::stop(void)
{
    vl53l5cx_stop_ranging(&_dev);
}