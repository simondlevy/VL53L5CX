<a href="https://www.tindie.com/products/onehorse/vl53l5cx-ranging-camera/">
<img src="media/vl53l5.jpg" width=400>
</a>

# VL53L5
Arduino library for ST Microelectronics VL53L5 multizone Time-of-Flight ranging camera
(get it [here](https://www.tindie.com/products/onehorse/vl53l5cx-ranging-camera/)!)

## Quickstart

1. Connect I<sup>2</sup>C in the usual way (SCL, SDA, 3.3V, GND)

2. Connect INT and LPN to digital pins on your board, making sure the pin
numbers agree with the [pin numbers](https://github.com/simondlevy/VL53L5/blob/main/examples/GetSetParams/GetSetParams.ino#L14-L15) specified in the 
<b>GetSetParams</b> test sketch.

3. Flash the <b>GetSetParams</b> test sketch onto your board.

4. After a few seconds you should see the LED turn on solid.

5. A few seconds after that, the LED should start blinking.  

6. Open the serial monitor to observer the rangefinder readings.

## Supported platforms

I have tested this library on the following platforms:

* [Ladybug STM32L432 Development Board](https://www.tindie.com/products/TleraCorp/ladybug-stm32l432-development-board/) from Tlera Corp.

* [Teensy 3.2 Develeopment Board](https://www.pjrc.com/store/teensy32.html) from PJRC

* [Teensy 4.0 Develeopment Board](https://www.pjrc.com/store/teensy40.html) from PJRC

For those working in the stm32duino framework, I recommend this [library](https://github.com/stm32duino/VL53L5CX).
