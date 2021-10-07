#!/usr/bin/env python3
'''
Displays 8x8 images from VL53L5 sent over serial port

Copyright (c) 2021 Simon D. Levy

MIT License
'''

import argparse

parser = argparse.ArgumentParser()

parser.add_argument(dest='port', help='COM port')

args = parser.parse_args()

