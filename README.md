<a href="https://www.tindie.com/products/onehorse/vl53l5cx-ranging-camera/">
<img src="media/vl53l5.jpg" width=400>
</a>

# VL53L5
A lightweight, header-only Arduino library for ST Microelectronics VL53L5
multizone Time-of-Flight ranging camera (get it
[here](https://www.tindie.com/products/onehorse/vl53l5cx-ranging-camera/)!)
This library is designed to simplify the use of the sensor by helping you avoid
illegal configurations; e.g., specifying an integration constant in continuous mode,
or specifying too high a sampling frequency at a given resolution.

## Quickstart

1. Connect I<sup>2</sup>C in the usual way (SCL, SDA, 3.3V, GND)

2. Connect INT and LPN to digital pins on your board, making sure the pin
numbers agree with the
[pin numbers](https://github.com/simondlevy/VL53L5/blob/main/examples/Basic/Basic.ino#L13-L18)
specified in the <b>Basic</b> test sketch.

3. Flash the <b>Basic</b> test sketch onto your board.

4. After a few seconds you should see the LED turn on solid.

5. A few seconds after that, the LED should start blinking.  

6. Open the serial monitor to observer the rangefinder readings.

## Supported platforms

I have tested this library on the following platforms:

* [Ladybug STM32L432 Development Board](https://www.tindie.com/products/TleraCorp/ladybug-stm32l432-development-board/)

* [Teensy 3.2 Develeopment Board](https://www.pjrc.com/store/teensy32.html) 

* [Teensy 4.0 Develeopment Board](https://www.pjrc.com/store/teensy40.html)

* [TinyPICO ESP32 Develeopment Board](https://www.tinypico.com) 

## Related projects

* [SparkFun VL53L5CX Arduino Library](https://github.com/sparkfun/SparkFun_VL53L5CX_Arduino_Library)

* [stm32duino VL53L5CX](https://github.com/stm32duino/VL53L5CX)
