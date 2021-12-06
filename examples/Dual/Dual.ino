/*
 *  VL53L5CX dual-sensor example with settable I^2C addresses
 *
 *  Copyright (c) 2021 Kris Winer and Simon D. Levy
 *
 *  MIT License
 */

#include <Wire.h>

#include <VL53L5cx.h>
#include <Debugger.hpp>
#include "st/vl53l5cx_api.h"


// Standard
static const uint8_t LED_PIN = 13;

// Constants to fiddle with ------------------------

// Min freq is 1 Hz max is 15 Hz (8 x 8) or 60 Hz (4 x 4) Sum of integration
// time (1x for 4 x 4 and 4x for 8 x 8) must be 1 ms less than 1/freq,
// otherwise data rate decreased so integration time must be > 18 ms at 4x4, 60
// Hz, for example the smaller the integration time, the less power used, the
// more noise in the ranging data
static const uint8_t VL53L5_freq = 1;

// Time in milliseconds, settable only when in autonomous mode, otherwise a no op
static const uint8_t VL53L5_intTime = 10;

// --------------------------------------------------

// Ad-hoc class for supporting multiple VL53L5cx sensors with interrupts
class MultiSupport {

    private:

        uint8_t _id; // for reportig
        uint8_t _address;
        uint8_t _int_pin;
        uint8_t _lpn_pin;
        VL53L5CX_Configuration _dev;

    public:

        // Set by interrupt service routine
        volatile bool int_flag;

        MultiSupport(uint8_t id, uint8_t address, uint8_t int_pin, uint8_t lpn_pin)
        {
            _id = id;
            _address = address;
            _int_pin = int_pin;
            _lpn_pin = lpn_pin;
            int_flag = false;
        }

        void init(void (*isr)(void))
        {
            pinMode(_lpn_pin, OUTPUT);
            _dev.platform.address = 0x29;

            pinMode(_int_pin, INPUT);
            attachInterrupt(_int_pin, isr, FALLING);
        }

        void start(void)
        {
            // Make sure there is a VL53L5CX_0 sensor connected
            uint8_t isAlive = 0;
            uint8_t error = vl53l5cx_is_alive(&_dev, &isAlive);
            if (!isAlive || error) {
                Debugger::reportForever("VL53L5CX not detected at requested address");
            }

            // Init VL53L5CX_0 sensor
            error = vl53l5cx_init(&_dev);
            if (error) {
                Debugger::reportForever("VL53L5CX_%d ULD Loading failed");
            }

            Debugger::printf("VL53L5CX_ULD ready ! (Version : %s)\n",
                    VL53L5CX_API_REVISION);

            error = vl53l5cx_set_resolution(&_dev, VL53L5CX_RESOLUTION_4X4);
            if (error) {
                Debugger::printf("vl53l5CX_set_resolution failed, error %u\n", error);
            }

            // set autonomous ranging mode //
            error = vl53l5cx_set_ranging_mode(&_dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
            if (error) {
                Debugger::printf("VL53L5cX_set_ranging_mode failed, error %u\n", error);
            }

            // can set integration time in autonomous mode //
            error = vl53l5cx_set_integration_time_ms(&_dev, VL53L5_intTime); //  
            if (error) {
                Debugger::printf("VL53L5CX_set_integration_time_ms failed, error %u\n", error);
            }

            // Select data rate //
            error = vl53l5cx_set_ranging_frequency_hz(&_dev, VL53L5_freq);
            if (error) {
                Debugger::printf("VL53L5CX_set_ranging_frequency_hz failed, error %u\n", error);
            }

            // Set target order to closest //
            error = vl53l5cx_set_target_order(&_dev, VL53L5CX_TARGET_ORDER_CLOSEST);
            if (error) {
                Debugger::printf("VL53L5cx_set_target_order failed, error %u\n", error);
            }

            // Get current integration time //
            uint32_t integration_time_ms = 0;
            error = vl53l5cx_get_integration_time_ms(&_dev, &integration_time_ms);
            if (error) {
                Debugger::printf("VL53L5cx_get_integration_time_ms failed, error %u\n", error);
            }

            Debugger::printf("Current integration time is : %d ms\n", integration_time_ms);

            error = vl53l5cx_start_ranging(&_dev);
            if (error !=0) {
                Debugger::printf("start error = 0x%02X", error);
            }

            uint8_t isReady = 0;
            vl53l5cx_check_data_ready(&_dev, &isReady); // clear the interrupt

        } // run

        void enable(void)
        {
            digitalWrite(_lpn_pin, HIGH);
        }

        void disable(void)
        {
            digitalWrite(_lpn_pin, LOW);
        }

        void setAddress(uint8_t address)
        {
            vl53l5cx_set_i2c_address(&_dev, address<<1);
            _dev.platform.address = address;
        }

        void checkAndReport(void)
        {
            if (int_flag) {

                int_flag = false;

                uint8_t isReady = 0;

                vl53l5cx_check_data_ready(&_dev, &isReady);

                if (isReady) {

                    VL53L5CX_ResultsData results = {}; 

                    vl53l5cx_get_ranging_data(&_dev, &results);

                    // As the sensor is set in 4x4 mode by default, we have a total of
                    // 16 zones to print. For this example, only the data of first zone
                    // are printed 
                    Debugger::printf("VL53L5CX_%d data no : %3u\n", _id, _dev.streamcount);

                    for(uint8_t i=0; i<16; i++) {

                        Debugger::printf(
                                "Zone : %3d, Status : %3u, Distance : %4d mm\n",
                                i,
                                results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
                                results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
                    }
                    Debugger::printf("\n");
                }
            }
        }

}; 

static MultiSupport sensor0 = MultiSupport(0, 0x27, 5, 9);
static MultiSupport sensor1 = MultiSupport(1, 0x29, 8, 4);

static void isr0()
{
    sensor0.int_flag = true;
} 

static void isr1() 
{
    sensor1.int_flag = true;
} 

void setup(void)
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // start with led on, active HIGH

    Wire.begin();                    // Start I2C
    Wire.setClock(400000);           // Set I2C frequency at 400 kHz  
    delay(1000);

    sensor0.init(isr0);
    sensor1.init(isr1);

    sensor0.disable();
    sensor1.enable();

    sensor1.setAddress(0x27);

    sensor0.enable();

    delay(100);

    sensor0.start();
    sensor1.start();

    digitalWrite(LED_PIN, LOW); // turn off led when initiation successful

} // setup

void loop(void)
{
    sensor0.checkAndReport();
    sensor1.checkAndReport();

    // Blinkety blink!
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW); 
} 
