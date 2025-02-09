#include "ExternalInterrupterManager.h"

void ExternalInterrupterManager::setInterrupt(Interrupters Interrupter, bool on)
{
    switch (Interrupter){
        case Interrupters::Interrupter0:
            if (on){
                setInt0On();
            }
            else{
                setInt0Off();
            }
            break;

        case Interrupters::Interrupter1:
            if (on){
                setInt1On();
            }
            else{
                setInt1Off();
            }
            break;

        case Interrupters::Interrupter2:
            if (on){
                setInt2On();
            }
            else{
                setInt2Off();
            }
            break;

        default:
            break;
    }
}

bool ExternalInterrupterManager::ReadInterrupterPort(Interrupters Interrupter)
{
    bool returnValue=false;
    _delay_ms(DELAY_DEBOUNCE);
    switch (Interrupter){
        case Interrupters::Interrupter0:
            returnValue=(~PIND&PD2);
            break;

        case Interrupters::Interrupter1:
            returnValue=(~PIND&PD3);
            break;

        case Interrupters::Interrupter2:
            returnValue=(~PIND&PB2);
            break;

        default:
            return false;
    }

    return returnValue;
}

ExternalInterrupterManager ExternalInterrupterManager::myInstance_=
ExternalInterrupterManager();

ExternalInterrupterManager& ExternalInterrupterManager::getInstance()
{    
    return myInstance_;
}

void ExternalInterrupterManager::setInt0On()
{
    cli();
    DDRD &= ~(1 << PD2);
    EIMSK |= (1 << INT0);
    EICRA |= (EICRA & ~(1<<ISC01))|(1<<ISC00);
    sei();
}

void ExternalInterrupterManager::setInt0Off()
{
    cli();
    EIMSK &= ~(1 << INT0);
    sei();
}

void ExternalInterrupterManager::setInt1On()
{
    cli();
    DDRD &= ~(1 << PD3);
    EIMSK |= (1 << INT1);
    EICRA |= (EICRA & ~(1<<ISC11))|(1<<ISC10);
    sei();
}

void ExternalInterrupterManager::setInt1Off()
{
    cli();
    EIMSK &= ~(1 << INT1);
    sei();
}

void ExternalInterrupterManager::setInt2On()
{
    cli();
    DDRD &= ~(1 << PB2);
    EIMSK |= (1 << INT2);
    EICRA |= (EICRA & ~(1<<ISC21))|(1<<ISC20);
    sei();
}

void ExternalInterrupterManager::setInt2Off()
{
    cli();
    EIMSK &= ~(1 << INT2);
    sei();
}
