/*
*  VL53L5CX ULD basic example    
*
*  Copyright (c) 2021 Kris Winer, Simon D. Levy
*
*  MIT License
*/

#include "Wire.h"

#include "Debugger.hpp"
#include "vl53l5cx_api.h"

  static const uint8_t myLed  = 13;
  static const uint8_t INT_PIN = 8;
  static const uint8_t LPN_PIN = 3;

  uint8_t         status, isAlive = 0, isReady, i, error;
  uint32_t        integration_time_ms;
  
  volatile bool VL53L5_intFlag = false;

static VL53L5CX_Configuration Dev = {};  // Sensor configuration
static VL53L5CX_ResultsData Results = {};  // Results data from VL53L5CX


void setup(void)
{
    // Start serial debugging
    Serial.begin(115200);
    delay(4000);
    Serial.println("Serial begun!");

    pinMode(myLed, OUTPUT);
    digitalWrite(myLed, HIGH); // start with led on, actibe HIGH

    pinMode(INT_PIN, INPUT); // VL53L5CX interrupt pin

    // Start I^2C
    Wire.begin();
    Wire.setClock(400000); // I2C frequency at 400 kHz  
    delay(1000);

    I2Cscan(); // should show 0x29 for the VL53L5CX on the I2C bus

    Debugger::printf("starting\n\n");

    delay(1000);

    // Fill the platform structure with customer's implementation. For this
    // example, only the I2C address is used.
    Dev.platform.address = 0x29;

    // Reset the sensor by toggling the LPN pin
    Reset_Sensor(LPN_PIN);

 /* (Optional) Set a new I2C address if the wanted address is different
  * from the default one (filled with 0x20 for this example).
  */
  //status = vl53l5cx_set_i2c_address(&Dev, 0x20);

    // Check if there is a VL53L5CX sensor connected
    error = vl53l5cx_is_alive(&Dev, &isAlive);
    Serial.print("isAlive = 0x"); Serial.println(isAlive, HEX);
    Serial.print("error = 0x"); Serial.println(error, HEX);
    if(!isAlive || error) {
        Debugger::reportForever("VL53L5CX not detected at requested address");
    }

    // (Mandatory) Init VL53L5CX sensor
    error = vl53l5cx_init(&Dev);
    Serial.print("error = 0x"); Serial.println(error, HEX);
    if(error) {
        Debugger::reportForever("VL53L5CX ULD Loading failed");
    }

    Debugger::printf("VL53L5CX ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

    digitalWrite(myLed, LOW); // turn off led when initiation successfull

  /*********************************/
  /*        Set some params        */
  /*********************************/

  /* Set resolution in 8x8. WARNING : As others settings depend to this
   * one, it must be the first to use.
   */
  status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
  if(status)
  {
    printf("vl53l5cx_set_resolution failed, status %u\n", status);
  }

  /* Set ranging frequency to 1Hz.
   * Using 4x4, min frequency is 1Hz and max is 60Hz
   * Using 8x8, min frequency is 1Hz and max is 15Hz
   */
  status = vl53l5cx_set_ranging_frequency_hz(&Dev, 1);
  if(status)
  {
    printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
  }

  /* Set target order to closest */
  status = vl53l5cx_set_target_order(&Dev, VL53L5CX_TARGET_ORDER_CLOSEST);
  if(status)
  {
    printf("vl53l5cx_set_target_order failed, status %u\n", status);
  }

  /* Get current integration time */
  status = vl53l5cx_get_integration_time_ms(&Dev, &integration_time_ms);
  if(status)
  {
    printf("vl53l5cx_get_integration_time_ms failed, status %u\n", status);
  }
  printf("Current integration time is : %d ms\n", integration_time_ms);

  attachInterrupt(INT_PIN, VL53L5_intHandler, FALLING);

  error = vl53l5cx_start_ranging(&Dev);
  if(error !=0) {Serial.print("start error = "); Serial.println(error);}


 } /* end of setup */


void loop(void)
{
    if (VL53L5_intFlag) {
        VL53L5_intFlag = false;

        isReady = 0, error = 0;
        while(isReady == 0){
           error = vl53l5cx_check_data_ready(&Dev, &isReady);
        if(error !=0) {Serial.print("ready error = "); Serial.println(error);}
        delay(10);
        }

    if(isReady)
    {
      vl53l5cx_get_ranging_data(&Dev, &Results);

      /* As the sensor is set in 8x8 mode, we have a total
       * of 64 zones to print. For this example, only the data of
       * first zone are print */
      printf("Print data no : %3u\n", Dev.streamcount);
         for(i = 0; i < 64; i++)
         {
          printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
             i,
             Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
             Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
         }
      printf("\n");
    }
    } // end of VL53L5CX interrupt handling

    digitalWrite(myLed, HIGH); delay(10); digitalWrite(myLed, LOW);

    STM32.sleep();
} /* end of main loop */


void VL53L5_intHandler(){
  VL53L5_intFlag = true;
}


// I2C scan function
void I2Cscan()
{
// scan for i2c devices
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmission to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 //     error = Wire.transfer(address, NULL, 0, NULL, 0);
      
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
    
}
