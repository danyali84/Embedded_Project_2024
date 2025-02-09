#include "MotorManager.h"
#include "TimerManager.h"

MotorManager MotorManager::myInstance_=MotorManager();
MotorManager& MotorManager::getInstance()
{
    return myInstance_;
}

void MotorManager::setLeftSpeed(uint16_t speed)
{
    if (timer_.getMode(TimerManager::Timers::TIMER1) != TimerManager::Modes::PWM){
        timer_.setTimer(
            TimerManager::Timers::TIMER1,
            TimerManager::Modes::PWM,
            TimerManager::Prescalers::SCALE8
        );
    }
    timer_.setValueA(TimerManager::Timers::TIMER1, speed);
}

void MotorManager::setRightSpeed(uint16_t speed)
{
    if (timer_.getMode(TimerManager::Timers::TIMER1) != TimerManager::Modes::PWM){
        timer_.setTimer(
            TimerManager::Timers::TIMER1,
            TimerManager::Modes::PWM,
            TimerManager::Prescalers::SCALE8
        );
    }
    timer_.setValueB(TimerManager::Timers::TIMER1,speed);
}

void MotorManager::setLeftDirection(bool direction)
{
    DDRD|=(1 << PD6);
    if (direction){
        PORTD |= (1 << PD6);
    }
    else {
        PORTD &= ~(1 << PD6);
    }
}

void MotorManager::setRightDirection(bool direction)
{
    DDRD|=(1 << PD3);
    if (direction){
        PORTD |= (1 << PD3);
    }
    else {
        PORTD &= ~(1 << PD3);
    }
}

void MotorManager::reset(){
    setRightDirection(false);
    setLeftDirection(false);
    setRightSpeed(0);
    setLeftSpeed(0);
}

TimerManager MotorManager::timer_= TimerManager::getInstance();