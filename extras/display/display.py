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

parser = argparse.ArgumentParser()

parser.add_argument(dest='port', help='COM port')

args = parser.parse_args()

port = serial.Serial(args.port, 115200)

while True:

    buf = port.read(64)

    image = np.reshape(np.frombuffer(buf, np.uint8), (8,8))

    image = cv2.resize(image, (400,400), interpolation= cv2.INTER_LINEAR)

    cv2.imshow('VL53L5', image)

    if cv2.waitKey(1) == 27:
        break

