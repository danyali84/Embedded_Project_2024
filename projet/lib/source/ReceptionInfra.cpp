#include "ReceptionInfra.h"
#include "memoire_24.h"

ReceptionInfra ReceptionInfra::myInstance_=ReceptionInfra();

ReceptionInfra& ReceptionInfra::getInstance(){
    return myInstance_;
}
ReceptionInfra::ReceptionInfra(){
    DDRC&=~(1<<PC4);
    PORTC|=(1<<PC4);
}

bool ReceptionInfra::isReceptor1(){
    return bit_is_clear(PINC,PC4);
}


bool isBuffer()   
{

    while (bit_is_set(PINC, PC4))
        ;

    uint16_t duration = 0;
    while (bit_is_clear(PINC, PC4))
    {
        _delay_us(10); 
        duration ++;
    }

    return (duration >= 240 - 30 && duration <= 240 + 30);

}


uint8_t isOne()
{
    while (bit_is_set(PINC,PC4));

    uint16_t duration = 0;
    while (bit_is_clear(PINC, PC4))
    {
        _delay_us(10); 
        duration ++;
    }
    return (duration >= 100 && duration <= 140);

}

uint8_t ReceptionInfra::receiveOne(){
    uint8_t value=0;

    while (!isBuffer());

    _delay_us(400);

    //get values
    for (uint8_t i=0;i<8;i++){

        if (isOne()){
            value|=(1<<i);
        }

        _delay_us(550);
    }
    _delay_ms(10);
    return value;
}

bool ReceptionInfra::reception(){
    uint8_t value=0;
    uint16_t crc=0xFFFF;

    for (uint16_t i=3000; i<3000+N_DATA;i++){
        value=receiveOne();

        crc = _crc16_update(crc,value);
        
        mem_.ecriture(i,value);
        _delay_ms(READ_WRITE_DELAY);
    }

    uint16_t crcIncomming=receiveOne();
    crcIncomming=(crcIncomming<<8)|receiveOne();

    return crc==crcIncomming;
}