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
#include <avr/interrupt.h>

class ExternalInterrupterManager
{
public:
    enum class Interrupters{Interrupter0, Interrupter1, Interrupter2};
    
    void setInterrupt(Interrupters Interrupter, bool on);
    
    bool ReadInterrupterPort(Interrupters Interrupter);
    
    static ExternalInterrupterManager& getInstance();
    
private:
    static ExternalInterrupterManager myInstance_;
    static const uint8_t DELAY_DEBOUNCE = 30;

    void setInt0On();
    void setInt0Off();

    void setInt1On();
    void setInt1Off();

    void setInt2On();
    void setInt2Off();

    ExternalInterrupterManager()=default;
    
};