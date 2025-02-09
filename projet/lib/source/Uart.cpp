#include "Uart.h"


Uart::Uart() {
    UBRR0H = 0;

    UBRR0L = 0xCF;

    UCSR0A = (1<<UDRE0) ;

    UCSR0B = (1<<TXEN0) | (1<<RXEN0);

    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01) | (1<<UCSZ02);
}

void  Uart::transmissionUart( unsigned char data ) 
{
    while (!( UCSR0A & (1<<UDRE0)) );
    UDR0 = data;
    
}

void Uart::transmit(char* data, uint8_t longueur)
{
    for ( uint8_t i = 0; i < longueur; i++ ) {
        transmissionUart ( *(data+i) );
        _delay_ms(2);
    }
}
uint8_t  Uart::receive() 
{
    while (!(UCSR0A & (1<<RXC0))); 
    return UDR0; 
}

Uart Uart::myInstance_=Uart();

Uart& Uart::getInstance()
{
    return myInstance_;
}