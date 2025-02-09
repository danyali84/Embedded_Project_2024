// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: final project- ReceptionInfra
// Description: The goal of the program is to receive data through the infrared receptor
// Ports Used:c2 on robot 1 only
#pragma once

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdint.h>
#include <util/crc16.h>
#include "LedManager.h"
#include "memoire_24.h"

class ReceptionInfra{
public:
    static ReceptionInfra& getInstance();
    //the return is used to check if the communication was right
    bool reception();

//private:

    static const uint16_t INITIAL_TRANFER_DELAY=2400;
    static const uint16_t ZERO_TRANFER_DELAY=600;
    static const uint16_t ONE_TRANFER_DELAY=1200;
    static const uint16_t BETWEEN_TRANFER_DELAY=600;
    static const uint8_t  READ_WRITE_DELAY = 5; 

    static ReceptionInfra myInstance_;
    Memoire24CXXX mem_=Memoire24CXXX();
    LedManager led = LedManager::getInstance();
    ReceptionInfra();
    uint8_t receiveOne();
    bool isReceptor1();
    
    const uint16_t N_DATA=12; //a changer pour une valeur hard coder
};