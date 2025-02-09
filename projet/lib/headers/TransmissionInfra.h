// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: final project- ReceptionInfra
// Description: The goal of the program is to transmit data through the infrared receptor
// Ports Used:c2 on robot 2 only
#pragma once

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdint.h>
#include <util/crc16.h>
#include "memoire_24.h"
#include "TimerManager.h"

class TransmissionInfra{
public:
    void transmit(uint8_t* values); //a changer pour lecture en memoire
    static TransmissionInfra& getInstance();
    void ledOn();
    void ledOff();
private:

    static TransmissionInfra myInstance_;
    Memoire24CXXX mem_;
    void transmit(uint8_t value);
    TransmissionInfra();
    static const uint16_t INITIAL_TRANFER_DELAY=2400;
    static const uint16_t ZERO_TRANFER_DELAY=600;
    static const uint16_t ONE_TRANFER_DELAY=1200;
    static const uint16_t BETWEEN_TRANFER_DELAY=600;
    static const uint16_t  READ_WRITE_DELAY = 800; 
                                         

    static TimerManager timer_;
    const uint16_t N_DATA=12;// a changer pour une valeur hard coder
};