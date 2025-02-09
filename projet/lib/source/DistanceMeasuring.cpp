#include "DistanceMeasuring.h"

//reminders:
//A0 port is used
//tp6 uses can example

DistanceMeasuring DistanceMeasuring::myInstance_=DistanceMeasuring();
DistanceMeasuring& DistanceMeasuring::getInstance()
{
    return myInstance_;
}

uint8_t DistanceMeasuring::readDistance(){
    uint16_t value = 0;
    for (int i = 0; i < 10; i++){
        _delay_ms(10);
        value += can_.read() >> 2;
    }

    return value/10;
}

DistanceMeasuring::DistanceMeasuring() {
    DDRA &= ~(1<<PA0);
}