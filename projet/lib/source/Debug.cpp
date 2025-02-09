#include "Debug.h"


void printDebugStr(char* string, uint8_t length){
    Uart uart=Uart::getInstance();
    uart.transmit(string,length);
}
void printDebugInt(uint16_t value){
    Uart uart=Uart::getInstance();
    char digits[20];
    itoa(value, digits,10);
    uart.transmit(digits,strlen(digits));
    char chr='\n';
    uart.transmit(&chr,1);
}