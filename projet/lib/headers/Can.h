// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - Can
// Description: The program aims to convert an analogic signal into a numeric format.
// Ports Used: Port A essentially used as an analogic input


#ifndef CAN_H
#define CAN_H

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

class Can
{
public:

   static Can& getInstance();
   
   ~Can();
   uint16_t read();

private:
   static Can myInstance_;
   Can();
};

#endif