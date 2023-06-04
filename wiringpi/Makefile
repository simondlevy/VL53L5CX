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
STSRC = $(SRC)/st

all: $(ALL)

basic: basic.o vl53l5cx_api.o platform.o
	g++ -std=c++11 -o basic basic.o vl53l5cx_api.o platform.o # -lwiringPi

basic.o: basic.cpp 
	g++ -std=c++11 -Wall -I$(SRC) -c basic.cpp

vl53l5cx_api.o: $(STSRC)/vl53l5cx_api.cpp 
	g++ -std=c++11 -Wall -I$(SRC) -c $(STSRC)/vl53l5cx_api.cpp

platform.o: platform.cpp 
	g++ -std=c++11 -Wall -I$(SRC) -I$(STSRC) -c platform.cpp

run: basic
	./basic

edit:
	vim basic.cpp

clean:
	rm -f $(ALL) *.o *~
