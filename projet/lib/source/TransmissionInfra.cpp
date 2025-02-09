#include "TransmissionInfra.h"

TransmissionInfra TransmissionInfra::myInstance_ = TransmissionInfra();

TransmissionInfra &TransmissionInfra::getInstance()
{
    return myInstance_;
}

TimerManager TransmissionInfra::timer_ = TimerManager::getInstance();

void TransmissionInfra::transmit(uint8_t* values)
{ // a changer pour lecture en memoire
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 3000; i <3000+N_DATA ; i++) //N_DATA IS PUT HERE
    {
        crc = _crc16_update(crc, values[i]);
        transmit(values[i]);
        _delay_ms(READ_WRITE_DELAY);
    }
    _delay_ms(READ_WRITE_DELAY);
    transmit(crc >> 8);
    _delay_ms(READ_WRITE_DELAY);
    transmit((crc << 8) >> 8);
}

void TransmissionInfra::transmit(uint8_t value)
{
    ledOn();
    _delay_us(INITIAL_TRANFER_DELAY);
    ledOff();
    _delay_us(BETWEEN_TRANFER_DELAY);
    for (uint8_t i = 0; i < 8; i++)
    {
        ledOn();
        if (value & (1 << i))
        {
            _delay_us(ONE_TRANFER_DELAY);
        }
        else
        {
            _delay_us(ZERO_TRANFER_DELAY);
        }
        ledOff();
        _delay_us(BETWEEN_TRANFER_DELAY);
    }
}

TransmissionInfra::TransmissionInfra()
{
}

void TransmissionInfra::ledOn()
{
    DDRB |= (1<<PB3);
    TCNT0=0;
    uint8_t compare = 104;
    timer_.setTimer(
        TimerManager::Timers::TIMER0, 
        TimerManager::Modes::CTC_1_COMPARE, 
        TimerManager::Prescalers::SCALE1, 
        compare);

}

void TransmissionInfra::ledOff()
{
    if(!(PINB & (1<<PB3))){
        TCNT0=104;
    }else{
        TCNT0=103;
    }
    uint8_t compare = 104;
    DDRB &= ~(1<<PB3);
    timer_.setTimer(
        TimerManager::Timers::TIMER0, 
        TimerManager::Modes::CTC_1_COMPARE, 
        TimerManager::Prescalers::SCALE1, 
        compare);
    
}
