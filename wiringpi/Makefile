# 
# Makefile for VL53L5 WiringPi example
#
# Copyright (c) 2023 Simon D. Levy
#
# MIT License

  
# Change this to whereever you installed CrossPlatformDataBus
CPDB = $(HOME)/CrossPlatformDataBus

ALL = basic

LIS2MDLSRC = ../../src

CPSRC  = $(CPDB)/src
CPEXT  = $(CPDB)/extras
CPCMN  = $(CPEXT)/common
I2CSRC = $(CPEXT)/i2c/wiringpi/src

all: $(ALL)

basic: basic.o 
	g++ -std=c++11 -o basic basic.o -lwiringPi

basic.o: basic.cpp 
	g++ -std=c++11 -Wall -I$(CPSRC) -c basic.cpp

run: basic
	./basic

edit:
	vim basic.cpp

clean:
	rm -f $(ALL) *.o *~
