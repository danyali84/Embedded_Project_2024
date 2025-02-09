// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - MotorManager
// Description: The goal of the program is to control the wheels of the robot through a class
// Ports Used:D6 and D3 are used for direction, D4 and D5 are used for speed

#pragma once

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "TimerManager.h"

class MotorManager
{
public:
    static MotorManager& getInstance();

    void setLeftSpeed(uint16_t speed);
    void setRightSpeed(uint16_t speed);

    void setLeftDirection(bool direction);
    void setRightDirection(bool direction);

    void reset();

private:
    static MotorManager myInstance_;
    static TimerManager timer_;
    
    MotorManager()=default;
};

