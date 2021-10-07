#!/usr/bin/env python3
'''
Displays 8x8 images from VL53L5 sent over serial port

Copyright (c) 2021 Simon D. Levy

MIT License
'''

import argparse
import serial
import cv2
import numpy as np
from sys import stdout

def debug(s):
    print(s)
    stdout.flush()

parser = argparse.ArgumentParser()

parser.add_argument(dest='port', help='COM port')

args = parser.parse_args()

port = serial.Serial(args.port, 115200)

count = 0

image = np.zeros((8,8), dtype='uint8')
count = 0

while True:

    b = ord(port.read(1))

    if b == 0xFF:

        image = cv2.resize(image, (400,400), interpolation= cv2.INTER_NEAREST)

        cv2.imshow('VL53L5', image)

        if cv2.waitKey(1) == 27:
            break

        image = np.zeros((8,8), dtype='uint8')
        count = 0

    else:

        image[count//8, count%8] = b
        count += 1

