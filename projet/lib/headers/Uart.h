// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - Uart
// Description: The program's objective is to facilitate data transmission between the AVR microchip and other devices.
// Ports Used: built-in UART module

#pragma once

#include <avr/io.h>
#define F_CPU 8000000UL
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

class Uart
{
public:
    static Uart& getInstance();

    void transmit(char* data, uint8_t length);
    
    uint8_t receive();

private:
    static Uart myInstance_;

    void transmissionUart( unsigned char data );
    
    Uart();

};