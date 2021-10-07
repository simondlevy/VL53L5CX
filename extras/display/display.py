#!/usr/bin/env python3
'''
Displays 8x8 images from VL53L5 sent over serial port

Copyright (c) 2021 Simon D. Levy

MIT License
'''

import argparse
import serial
from sys import stdout

parser = argparse.ArgumentParser()

parser.add_argument(dest='port', help='COM port')

args = parser.parse_args()

port = serial.Serial(args.port, 115200)

while True:

    buf = port.read(64)

    print(len(buf))
    stdout.flush()
