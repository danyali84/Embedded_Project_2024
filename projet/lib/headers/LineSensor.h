// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - ExternalInterrupterManager
// Description: The goal of the program is to control interrupts through a class
// Ports Used:D2 is INT0, D3 is INT1 and B2 is INT2
//INT1 is the external button Interrupter which sends high when not pressed

#pragma once
#include <avr/io.h>
#define F_CPU 8000000UL
#include <stdint.h>
#include <util/delay.h>


class LineSensor{
public:
    static LineSensor& getInstance();
    bool sensorShortLeft();
    bool sensorFarLeft();
    bool sensorMid();
    bool sensorShortRight();
    bool sensorFarRight();
    bool all();

private:
    static LineSensor myInstance_;
    LineSensor();
};