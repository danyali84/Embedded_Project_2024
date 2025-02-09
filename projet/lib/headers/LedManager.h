// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - LedManager
// Description: The goal of the program is to control a led through a class
// Ports Used:B0 is green B1 is red

#pragma once

#include <avr/io.h>
#define F_CPU 8000000UL
#include <stdint.h>
#include <util/delay.h>

class LedManager
{
public:
    void setRed();

    void setGreen();

    void setAmber();

    void setOff();

    static LedManager& getInstance();

private:
    static LedManager myInstance_;   
    static const uint8_t AMBER_DELAY=1;
    
    LedManager();    
};