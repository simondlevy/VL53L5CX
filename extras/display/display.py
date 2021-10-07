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

SCALEUP = 50
FONT = cv2.FONT_HERSHEY_SIMPLEX
FONT_SIZE = 0.5
FONT_COLOR = (255,255,255)
FONT_THICKNESS = 1

def debug(s):
    print(s)
    stdout.flush()

def new_image():
    return np.zeros((8,8), dtype='uint8'), 0

parser = argparse.ArgumentParser()

parser.add_argument(dest='port', help='COM port')

args = parser.parse_args()

port = serial.Serial(args.port, 115200)

count = 0

image, count = new_image()

while True:

    b = ord(port.read(1))

    if b == 0xFF:

        resized = cv2.resize(image, (8*SCALEUP,8*SCALEUP), interpolation= cv2.INTER_NEAREST)

        for j in range(8):
            for k in range(8):
                cv2.putText(resized,
                            '%d' % image[k,j],
                            (j*SCALEUP+SCALEUP//4,k*SCALEUP+SCALEUP//2),
                            FONT, FONT_SIZE, FONT_COLOR, FONT_THICKNESS, cv2.LINE_AA)

        cv2.imshow('VL53L5 [ESC to quit]', resized)

        if cv2.waitKey(1) == 27:
            break

        image, count = new_image()

    else:

        image[count//8, count%8] = b
        count += 1

