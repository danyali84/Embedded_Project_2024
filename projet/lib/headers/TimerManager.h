// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - TimerManager
// Description: The goal of the program is to control a lead through a class
// Ports Used: DDRD or DDRB (for Timer0), the registers TCCRnA, TCCRnB, TCNTn, OCRnA, OCRnB, and TIMSKn.

#pragma once
#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

class TimerManager
{
public:
    static TimerManager& getInstance();

    enum class Modes{CTC_1_COMPARE, CTC_2_COMPARE, PWM, SOUND};
    enum class Prescalers {
            SCALE1,
            SCALE8,
            SCALE32,
            SCALE64,
            SCALE128,
            SCALE256,
            SCALE1024
        };
    enum class Timers{TIMER0, TIMER1, TIMER2};

    void setTimer(Timers timer, Modes mode, Prescalers scale, uint16_t valueA=0,
                                                              uint16_t valueB=0);
    void setValueA(Timers timer,uint16_t value);
    void setValueB(Timers timer,uint16_t value);

    Modes getMode(Timers timer);
    
private:
    Modes mode0_;
    Modes mode1_;
    Modes mode2_;

    static TimerManager myInstance_;
    
    void setModeTimer0(Modes mode);
    void setPrescalerTimer0(Prescalers prescaler);
    void setCompareATimer0(uint16_t value);
    void setCompareBTimer0(uint16_t value);

    void setModeTimer1(Modes mode);
    void setPrescalerTimer1(Prescalers prescaler);
    void setCompareATimer1(uint16_t value);
    void setCompareBTimer1(uint16_t value);

    void setModeTimer2(Modes mode);
    void setPrescalerTimer2(Prescalers prescaler);
    void setCompareATimer2(uint16_t value);
    void setCompareBTimer2(uint16_t value);

    TimerManager()=default;
};