3 examples in one

In Core/Src :
main_multi.c; // central sensor and left and right sensor
main_sat.c; // one satellite sensor connect to the STM32 Nucleo 
main_simple.c // only sensor on the expansion board

Include just one main_*.c at the time and exclude two other main_*.c
