# 
# Makefile for VL53L5 WiringPi example
#
# Copyright (c) 2023 Simon D. Levy
#
# MIT License

  
# Change this to whereever you installed CrossPlatformDataBus
CPDB = $(HOME)/CrossPlatformDataBus

ALL = basic

SRC = ../src

all: $(ALL)

basic: basic.o 
	g++ -std=c++11 -o basic basic.o # -lwiringPi

basic.o: basic.cpp 
	g++ -std=c++11 -Wall -I$(SRC) -c basic.cpp

run: basic
	./basic

edit:
	vim basic.cpp

clean:
	rm -f $(ALL) *.o *~
