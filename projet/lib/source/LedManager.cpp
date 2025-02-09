// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp7 - LedManager
// Description: The goal of the program is to control a lead through a class

#include "LedManager.h"

void LedManager::setRed()
{
    PORTC &= ~(1 << PC2);
    PORTC |= (1 << PC3);
}

void LedManager::setGreen()
{
    PORTC&= ~(1 << PC3);
    PORTC |= (1 << PC2);
}

void LedManager::setAmber()
{
    setGreen();
    _delay_ms(AMBER_DELAY);
    setRed();
    _delay_ms(AMBER_DELAY);
}

void LedManager::setOff(){

    PORTC &= ~((1 << PC2) | (1 << PC3));
}

LedManager LedManager::myInstance_=LedManager();

LedManager& LedManager::getInstance()
{
    return myInstance_;
}

LedManager::LedManager()
{
    DDRC|=(1<<PC2)|(1<<PC3);
}