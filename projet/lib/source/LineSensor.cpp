#include "LineSensor.h"



bool LineSensor::sensorFarLeft(){
     return (PINA & (1 << PA0));
}

bool LineSensor::sensorShortLeft(){
     return (PINA & (1 << PA1));
}

bool LineSensor::sensorMid(){
    return (PINA & (1 << PA2));
}

bool LineSensor::sensorShortRight(){
     return (PINA & (1 << PA3));
}
bool LineSensor::sensorFarRight(){
     return (PINA & (1 << PA4));
}
bool LineSensor::all(){
     return sensorFarLeft() && sensorShortLeft() && 
          sensorMid() && sensorShortRight() && sensorFarRight();
}

LineSensor LineSensor::myInstance_=LineSensor();

LineSensor& LineSensor::getInstance()
{
    return myInstance_;
}

LineSensor::LineSensor(){
    DDRA |= (1 << PA6) | (1 << PA7); 
    PORTA &= ~(1 << PA6); 
    PORTA |= (1 << PA7);
    DDRA &= ~(1 << PA0);
    DDRA &= ~(1 << PA1);
    DDRA &= ~(1 << PA2);
    DDRA &= ~(1 << PA3);
    DDRA &= ~(1 << PA4);
}
