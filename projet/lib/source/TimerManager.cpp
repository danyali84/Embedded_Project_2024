#include "TimerManager.h"

TimerManager TimerManager::myInstance_=TimerManager();

TimerManager& TimerManager::getInstance()
{
    return myInstance_;
}

void TimerManager::setModeTimer0(Modes mode)
{
    cli();
    TCCR0A &= ~(1 << WGM00) & ~(1 << WGM01);
    TCCR0B &= ~(1 << WGM02);
    TIMSK0 &= ~(1 << OCIE0A) & ~(1 << OCIE0B);
    switch(mode){
        case Modes::CTC_1_COMPARE:
            TCCR0A |= (1 << WGM01) | (1 << COM0A0);
            break;
        case Modes::CTC_2_COMPARE:
            TCCR0A |= (1 << WGM01);
            TIMSK0 |= (1 << OCIE0A);
            TIMSK0 |= (1 << OCIE0B);
            break;
        case Modes::PWM:
            TCCR0A |= (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
            DDRB |= (1 << PB3) | (1 << PB4);
            break;
        case Modes::SOUND:
            TCCR2A |= (1 << COM0A0) | (1 << WGM01);
            DDRD |= (1 << PD6) | (1 << PD7);
            PORTD &= ~(1 << PD6);
            break;
    }
    sei();

}

TimerManager::Modes TimerManager::getMode(Timers timer)
{
    switch(timer){
        case Timers::TIMER0:
            return mode0_;
            break;

        case Timers::TIMER1:
            return mode1_;
            break;

        case Timers::TIMER2:
            return mode2_;
            break;
    }
    return mode1_;
}

void TimerManager::setValueA(Timers timer,uint16_t value)
{
        switch(timer){
        case Timers::TIMER0:
            setCompareATimer0(value);
            break;

        case Timers::TIMER1:
            setCompareATimer1(value);
            break;

        case Timers::TIMER2:
            setCompareATimer2(value);
            break;
    }
}

void TimerManager::setValueB(Timers timer,uint16_t value)
{
        switch(timer){
        case Timers::TIMER0:
            setCompareBTimer0(value);
            break;

        case Timers::TIMER1:
            setCompareBTimer1(value);
            break;

        case Timers::TIMER2:
            setCompareBTimer2(value);
            break;
    }
}

void TimerManager::setPrescalerTimer0(Prescalers prescaler)
{
    {
    cli();
    TCCR0B &= ~((1 << CS00) | (1 << CS01) | (1 << CS02)); 
    switch (prescaler){
        case Prescalers::SCALE1:
            TCCR0B|= (1 << CS00);
            break;
        case Prescalers::SCALE8:
            TCCR0B|= (1 << CS01);
            break;
        case Prescalers::SCALE64:
            TCCR0B|=(1 << CS00) | (1 << CS01);
            break;
        case Prescalers::SCALE256:
            TCCR0B|= (1 << CS02);
            break;
        case Prescalers::SCALE1024:
            TCCR0B|= (1 << CS02) | (1 << CS00);
            break;
        default:
            break;
    }
    
    sei();
}
}
void TimerManager::setCompareATimer0(uint16_t valueA)
{
    
    OCR0A = valueA;
}
void TimerManager::setCompareBTimer0(uint16_t valueB)
{
    OCR0B = valueB;
}



void TimerManager::setModeTimer1(Modes mode)
{
    cli();
    TCNT1 = 0 ;
    TCCR1A &= ~(1 << WGM10) & ~(1 << WGM11);
    TCCR1B &= ~(1 << WGM12);
    TCCR1C = 0;
    TIMSK1 &= ~(1 << OCIE1A) & ~(1 << OCIE1B);
    switch(mode){
        case Modes::CTC_1_COMPARE:
            TCCR1A = 0;
            TIMSK1 |= (1 << OCIE1A);
            TCCR1B = (1<<WGM12);
            break;
        case Modes::CTC_2_COMPARE:
            TCCR1A |= (1 << WGM11);
            TIMSK1 |= (1 << OCIE1A);
            TIMSK1 |= (1 << OCIE1B);
            break;
        case Modes::PWM:
            TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
            DDRD |= (1 << PD4) | (1 << PD5);
            break;
        case Modes::SOUND:
            TCCR2A |= (1 << COM1A0) | (1 << WGM11);
            DDRD |= (1 << PD6) | (1 << PD7);
            PORTD &= ~(1 << PD6);
            break;
    }
    sei();
}
void TimerManager::setPrescalerTimer1(Prescalers prescaler)
{
    {
    cli();
    TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); 
    switch (prescaler){
        case Prescalers::SCALE1:
            TCCR1B|= (1 << CS10);
            break;
        case Prescalers::SCALE8:
            TCCR1B|= (1 << CS11);
            break;
        case Prescalers::SCALE64:
            TCCR1B|= (1 << CS10) | (1 << CS11);
            break;
        case Prescalers::SCALE256:
            TCCR1B= (1 << CS12);
            break;
        case Prescalers::SCALE1024:
            TCCR1B|= (1 << CS12) | (1 << CS10);
            break;
        default:
            break;
    }
    
    sei();
}
}
void TimerManager::setCompareATimer1(uint16_t valueA)
{
    OCR1A = valueA;
}
void TimerManager::setCompareBTimer1(uint16_t valueB)
{
    OCR1B = valueB;
}

void TimerManager::setModeTimer2(Modes mode)
{
    cli();
    TCCR2A &= ~(1 << WGM20) & ~(1 << WGM21);
    TCCR2B &= ~(1 << WGM22);
    TIMSK2 &= ~(1 << OCIE2A) & ~(1 << OCIE2B);
    switch(mode){
        case Modes::CTC_1_COMPARE:
            TCCR2A |= (1 << WGM21);
            TIMSK2 |= (1 << OCIE2A);
            break;
        case Modes::CTC_2_COMPARE:
            TCCR2A |= (1 << WGM21);
            TIMSK2 |= (1 << OCIE2A);
            TIMSK2 |= (1 << OCIE2B);
            break;
        case Modes::PWM:
            TCCR2A |= (1 << WGM20) | (1 << COM2A1) | (1 << COM2B1);
            DDRD |= (1 << PD6) | (1 << PD7);
            break;
        case Modes::SOUND:
            TCCR2A |= (1 << COM2A0) | (1 << WGM21);
            DDRD |= (1 << PD6) | (1 << PD7);
            PORTD &= ~(1 << PD6);
            break;
    }
    sei();
}
void TimerManager::setPrescalerTimer2(Prescalers prescaler)
{
    {
    cli();
    TCCR2B &= ~((1 << CS20) | (1 << CS21) | (1 << CS22));
    switch (prescaler){
        case Prescalers::SCALE1:
            TCCR2B|= (1 << CS20);
            break;
        case Prescalers::SCALE8:
            TCCR2B|= (1 << CS21);
            break;
        case Prescalers::SCALE32:
            TCCR2B|= (1 << CS20) | (1 << CS21);
            break;
        case Prescalers::SCALE64:
            TCCR2B|=(1 << CS22);
            break;
        case Prescalers::SCALE128:
            TCCR2B|= (1 << CS12);
            break;
        case Prescalers::SCALE256:
            TCCR2B|= (1 << CS22) | (1 << CS21);
            break;
        case Prescalers::SCALE1024:
            TCCR2B|=(1 << CS22) | (1 << CS21) | (1 << CS20);
            break;
    }
    
    sei();
}

}

void TimerManager::setCompareATimer2(uint16_t valueA)
{
    OCR2A = valueA;
}
void TimerManager::setCompareBTimer2(uint16_t valueB)
{
    OCR2B = valueB;
}

void TimerManager::setTimer(Timers timer, Modes mode, Prescalers scale, uint16_t valueA, uint16_t valueB){
    switch (timer){
        case Timers::TIMER0:
            mode0_=mode;
            setModeTimer0(mode);
            setPrescalerTimer0(scale);
            setCompareATimer0(valueA);
            setCompareBTimer0(valueB);
            break;
        case Timers::TIMER1:
            mode1_=mode;
            setModeTimer1(mode);
            setPrescalerTimer1(scale);
            setCompareATimer1(valueA);
            setCompareBTimer1(valueB);
            break;
        case Timers::TIMER2:
            mode2_=mode;
            setModeTimer2(mode);
            setPrescalerTimer2(scale);
            setCompareATimer2(valueA);
            setCompareBTimer2(valueB);
            break;
    }
}