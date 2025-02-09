// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - Debug
// Description: The "Debug" class provides conditional printing of strings and integers to a UART interface for debugging purposes


#pragma once

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Uart.h"

void printDebugStr(char* string, uint8_t length);
void printDebugInt(uint16_t value);

#ifdef DEBUG

#define DEBUG_PRINT_STR(x)  printDebugStr x
#define DEBUG_PRINT_INT(x)  printDebugInt x
#else
# define DEBUG_PRINT_STR(x) do {} while (false) 
# define DEBUG_PRINT_INT(x) do {} while (false)

#endif