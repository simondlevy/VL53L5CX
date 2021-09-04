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
        target_order_t targetOrder,
        uint8_t rangingFrequency)
{
    _lpn_pin = lpnPin;

    _dev.platform.address = deviceAddress;

    _resolution = resolution;
    _target_order = targetOrder;

    _ranging_frequency = rangingFrequency;
}
        
void VL53L5cx::begin(void)
{
    init();
    start_ranging();
}

void VL53L5cx::init(void)
{
    // Bozo filter for ranging frequency
    check_ranging_frequency(RESOLUTION_4X4, 60, "4X4");
    check_ranging_frequency(RESOLUTION_8X8, 15, "8X8");

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

} // init

void VL53L5cx::check_ranging_frequency(resolution_t resolution,
                                       uint8_t maxval,
                                       const char *label)
{
    if (_ranging_frequency < 1 || _ranging_frequency > maxval) {
        Debugger::reportForever("Ranging frequency for %s resolution " 
                "must be at least 1 and no more than %d", maxval);
    }
}


void VL53L5cx::start_ranging(void)
{
    uint8_t error = vl53l5cx_start_ranging(&_dev);
    if(error !=0) {
        Debugger::reportForever("start error = 0x%02X", error);
    }
}

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

uint32_t VL53L5cx::getIntegrationTimeMsec(void)
{
    uint32_t integration_time_ms = 0;
    uint8_t error = vl53l5cx_get_integration_time_ms(&_dev,
            &integration_time_ms);
    if (error) {
        Debugger::reportForever("vl53l5cx_get_integration_time_ms failed, status %u\n",
                error);

    }

    return integration_time_ms;
}

VL53L5cxAutonomous::VL53L5cxAutonomous(
        uint8_t lpnPin,
        uint32_t integrationTimeMsec,
        uint8_t deviceAddress,
        resolution_t resolution,
        target_order_t targetOrder)
    : VL53L5cxAutonomous::VL53L5cx(
            lpnPin,
            deviceAddress,
            resolution,
            targetOrder)
{
    _integration_time_msec = integrationTimeMsec;
}

void VL53L5cxAutonomous::begin(void)
{
    init();

    // Set ranging mode autonomous  
    uint8_t error = vl53l5cx_set_ranging_mode(&_dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
    if (error) {
        Debugger::reportForever("vl53l5cx_set_ranging_mode failed, status %u\n", error);
    }

    // Using autonomous mode, the integration time can be updated (not possible
    // using continuous)
    vl53l5cx_set_integration_time_ms(&_dev, _integration_time_msec);

    start_ranging();
}

