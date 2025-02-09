// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: Projet - DistanceMeasuring
// Description: 
// Ports Used:

#pragma once

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "DistanceMeasuring.h"
#include "Can.h"

class DistanceMeasuring
{
public:
    static DistanceMeasuring& getInstance();
    uint8_t readDistance();

private:
    static DistanceMeasuring myInstance_;
    Can can_=Can::getInstance();
    
    DistanceMeasuring();
};