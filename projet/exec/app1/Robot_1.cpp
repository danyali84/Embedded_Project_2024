#include <avr/io.h>
#define F_CPU 8000000UL
#include <stdint.h>
#include <util/delay.h>
#include "LedManager.h"
#include "ExternalInterrupterManager.h"
#include "memoire_24.h"
#include "Can.h"
#include "Uart.h"
#include "TimerManager.h"
#include "MotorManager.h"
#include "DistanceMeasuring.h"
#include "SoundManager.h"
#include "ReceptionInfra.h"


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Uart.h"

enum class State
{
    INIT,
    WAIT,
    BUTTON_PRESSED,
    RECEIVING_IR,
    WAIT_MAP,
    PRINTING_MAP
};

constexpr uint16_t WAIT_1_SECOND = 1000;
constexpr uint16_t WAIT_HALF_SECOND = 500;

//motors
    //distances
constexpr uint8_t MIDWAY_DISTANCE = 43;
constexpr uint8_t FACING_WALL_DISTANCE = 40;
constexpr uint8_t MIDWAY_STOP_LONGER = 32;
constexpr uint8_t MIDWAY_STOP_SHORTER = 48;

    //speeds
constexpr uint8_t TURNING_SPEED = 138;
constexpr uint8_t MOVE_STRAIGHT_LEFT = 121;
constexpr uint8_t MOVE_STRAIGHT_RIGHT = 120;

    //delays
constexpr uint16_t WAIT_MOTORS_90 = 900;
constexpr uint16_t WAIT_MOTORS_180 = 1500;


//memory
constexpr uint8_t MEMORY_DELAY = 10;


//addresses
const uint8_t WIDTH_ADDRESS = 102;
const uint8_t LENGTH_ADDRESS = 26;


//sounds
    //notes
constexpr uint8_t NOTE_HIGH_PITCHED_SOUND = 79;
constexpr uint8_t NOTE_LOW_PITCHED_SOUND = 45;

    //delays
const uint8_t SOUND_DELAY = 50;

    //iterators
constexpr uint8_t SOUND_ITERATIONS = 3;


uint8_t gInitialDistance;
bool gInitialPosition = true;
volatile bool gfrequence = true;
volatile State gCurrentState = State::INIT;

DistanceMeasuring &distanceMeasuring = DistanceMeasuring::getInstance();
MotorManager motors = MotorManager::getInstance();
SoundManager sound = SoundManager::getInstance();
LedManager led = LedManager::getInstance();

bool isButtonPressed()
{
    return !(PIND & (1 << PD2));
}

void rotate90()
{
    motors.setLeftDirection(true);
    motors.setLeftSpeed(TURNING_SPEED);
    motors.setRightSpeed(TURNING_SPEED);
    _delay_ms(WAIT_MOTORS_90);
    motors.setLeftSpeed(0);
    motors.setRightSpeed(0);
    motors.setLeftDirection(false);
}

void rotate180(){
    motors.setLeftDirection(true);
    motors.setLeftSpeed(TURNING_SPEED);
    motors.setRightSpeed(TURNING_SPEED);
    _delay_ms(WAIT_MOTORS_180);
    motors.setLeftSpeed(0);
    motors.setRightSpeed(0);
    motors.setLeftDirection(false);
}

void findingCenterBox()
{
    uint8_t distance = 0;
    distance = distanceMeasuring.readDistance();

    gInitialDistance = distance;

    motors.setLeftDirection(distance > MIDWAY_DISTANCE);
    motors.setRightDirection(distance > MIDWAY_DISTANCE);

    motors.setLeftSpeed(MOVE_STRAIGHT_LEFT);
    motors.setRightSpeed(MOVE_STRAIGHT_RIGHT);

    while ((distance < MIDWAY_STOP_LONGER) || (distance > MIDWAY_STOP_SHORTER))
    {
        distance = distanceMeasuring.readDistance();
    }

    motors.reset();

    _delay_ms(WAIT_HALF_SECOND);
    rotate90();
    _delay_ms(WAIT_HALF_SECOND);

    gInitialPosition = true;

    distance = distanceMeasuring.readDistance();
    if (distance > FACING_WALL_DISTANCE)
    {
        rotate180();

        gInitialPosition = false;
    }

    gCurrentState = State::WAIT;
}

void writeInMemory(uint8_t character, uint16_t address, Memoire24CXXX memory)
{

    memory.ecriture(address, character);
    _delay_us(MEMORY_DELAY);
}

uint16_t calculateAddress(uint8_t posX, uint8_t posY)
{
    uint16_t adress;
    adress = posX * WIDTH_ADDRESS + posY;
    return adress;
}

void writeHorizontalLine(Memoire24CXXX memory, uint8_t posX, uint8_t yBegin, uint8_t yEnd, uint8_t unicode)
{

    for (uint16_t i = calculateAddress(posX, yBegin); i <= calculateAddress(posX, yEnd); i++)
    {
        memory.ecriture(i, unicode);
        _delay_us(MEMORY_DELAY);
    }
}

void writeVerticalLine(Memoire24CXXX memory, uint8_t posY, uint8_t xBegin, uint8_t xEnd, uint8_t unicode)
{
    for (uint16_t i = calculateAddress(xBegin, posY); i <= calculateAddress(xEnd, posY); i += WIDTH_ADDRESS)
    {
        memory.ecriture(i, unicode);
        _delay_us(MEMORY_DELAY);
    }
}

void drawingRobot1(Memoire24CXXX memory, bool direction, uint8_t position)
{
    uint8_t adressePos = 0;
    if (direction)
    {
        adressePos = 18;
        adressePos -= 2 * position;
        writeInMemory(2, calculateAddress(adressePos, 20), memory);

        writeInMemory(20, calculateAddress(adressePos, 19), memory);
        writeInMemory(21, calculateAddress(adressePos, 21), memory);

        writeInMemory(3, calculateAddress(adressePos + 1, 22), memory);
        writeInMemory(3, calculateAddress(adressePos + 1, 18), memory);

        writeInMemory(29, calculateAddress(adressePos + 2, 22), memory);
        writeInMemory(29, calculateAddress(adressePos + 2, 18), memory);

        writeInMemory(22, calculateAddress(adressePos + 3, 22), memory);
        writeInMemory(23, calculateAddress(adressePos + 3, 18), memory);

        writeInMemory(2, calculateAddress(adressePos + 3, 20), memory);
        writeInMemory(2, calculateAddress(adressePos + 3, 21), memory);
        writeInMemory(2, calculateAddress(adressePos + 3, 19), memory);
    }
    else
    {
        adressePos = 7;
        adressePos += 2 * position;
        writeInMemory(2, calculateAddress(adressePos, 20), memory);

        writeInMemory(21, calculateAddress(adressePos, 19), memory);
        writeInMemory(20, calculateAddress(adressePos, 21), memory);

        writeInMemory(3, calculateAddress(adressePos - 1, 22), memory);
        writeInMemory(3, calculateAddress(adressePos - 1, 18), memory);

        writeInMemory(24, calculateAddress(adressePos - 3, 22), memory);
        writeInMemory(25, calculateAddress(adressePos - 3, 18), memory);

        writeInMemory(2, calculateAddress(adressePos - 3, 20), memory);
        writeInMemory(2, calculateAddress(adressePos - 3, 21), memory);
        writeInMemory(2, calculateAddress(adressePos - 3, 19), memory);

        writeInMemory(29, calculateAddress(adressePos - 2, 22), memory);
        writeInMemory(29, calculateAddress(adressePos - 2, 18), memory);
    }
}

void drawTemplate()
{
    Memoire24CXXX memory;
    uint16_t adress = 0x0000;
    uint8_t input = 0x00;
    uint8_t asciiSpace = 0x20;
    for (uint8_t i = 0; i < LENGTH_ADDRESS; ++i)
    {
        for (uint8_t j = 0; j < WIDTH_ADDRESS; ++j)
        {
            memory.ecriture(adress, asciiSpace);
            adress++;
            _delay_us(MEMORY_DELAY);
        }
    }

    uint16_t tempAdress = 0x0000;
    // top left corner
    tempAdress = calculateAddress(0, 0);
    input = 0;
    writeInMemory(input, tempAdress, memory);

    // top right corner
    tempAdress = calculateAddress(0, 101);
    input = 1;
    writeInMemory(input, tempAdress, memory);

    // bot letft corner
    tempAdress = calculateAddress(25, 0);
    input = 6;
    memory.ecriture(tempAdress, input);
    _delay_ms(MEMORY_DELAY);

    // bot right corner
    tempAdress = calculateAddress(25, 101);
    input = 7;
    memory.ecriture(tempAdress, input);
    _delay_ms(MEMORY_DELAY);

    // horizontal top line
    input = 2;
    writeHorizontalLine(memory, 0, 1, 100, input);

    // horizontal bot line
    writeHorizontalLine(memory, 25, 1, 100, input);

    // Vertical left line
    input = 3;
    writeVerticalLine(memory, 0, 1, 24, input);

    // Vertical right line
    writeVerticalLine(memory, 101, 1, 24, input);

    // horizontal line box
    input = 10;
    writeHorizontalLine(memory, 3, 7, 32, input);

    writeHorizontalLine(memory, 22, 7, 32, input);

    // vertical left line box
    input = 11;
    writeVerticalLine(memory, 7, 4, 21, input);

    // Vertical right line
    writeVerticalLine(memory, 33, 4, 21, input);
    writeVerticalLine(memory, 33, 12, 14, asciiSpace);

    // box corner top left
    tempAdress = calculateAddress(3, 7);
    input = 4;
    writeInMemory(input, tempAdress, memory);

    // box corner top right
    tempAdress = calculateAddress(3, 33);
    input = 5;
    writeInMemory(input, tempAdress, memory);

    // box corner bot left
    tempAdress = calculateAddress(22, 7);
    input = 8;
    writeInMemory(input, tempAdress, memory);

    // box corner bot right
    tempAdress = calculateAddress(22, 33);
    input = 9;
    writeInMemory(input, tempAdress, memory);

    // path begin
    input = 12;
    writeHorizontalLine(memory, 13, 37, 41, input);

    // path instersection
    tempAdress = calculateAddress(13, 42);
    input = 13;
    writeInMemory(input, tempAdress, memory);

    // path top left corner
    tempAdress = calculateAddress(5, 42);
    input = 14;
    writeInMemory(input, tempAdress, memory);

    // path bot left corner
    tempAdress = calculateAddress(20, 42);
    input = 15;
    writeInMemory(input, tempAdress, memory);

    // path top right corner
    tempAdress = calculateAddress(5, 93);
    input = 16;
    writeInMemory(input, tempAdress, memory);

    // path bot right corner
    tempAdress = calculateAddress(20, 93);
    input = 17;
    writeInMemory(input, tempAdress, memory);

    // horizontal line for path
    input = 18;
    writeHorizontalLine(memory, 5, 43, 92, input);

    writeHorizontalLine(memory, 20, 43, 92, input);

    // vertical line for path
    input = 19;
    writeVerticalLine(memory, 42, 6, 19, input);

    writeVerticalLine(memory, 93, 6, 19, input);
}

void addLineToMap(uint8_t position, uint8_t length)
{
    if (length == 0)
        return;
    Memoire24CXXX memory;
    length++;
    if (position <= 49)
    {
        position += 43;
        writeInMemory(27, calculateAddress(5, position), memory);
        writeVerticalLine(memory, position, 6, 5 + length, 19);
    }
    else
    {
        position = 142 - position;
        writeInMemory(26, calculateAddress(20, position), memory);
        writeVerticalLine(memory, position, 20 - length, 19, 19);
    }
}

void addSegmentsToMap(){
    Memoire24CXXX memoire;
    uint8_t length;
    uint8_t position;
    
    for (uint16_t address=3000; address<3012; address++){
        memoire.lecture(address,&position);
        address++;
        memoire.lecture(address,&length);
        addLineToMap(position, length);
    }   
}

void displayImage()
{
    Memoire24CXXX memory;
    uint8_t asciiSpace = 0x20;
    Uart &uart = Uart::getInstance();
    uint16_t adress = 0x0000;
    uint8_t codeMemory;
    // Table of convertion to Unicode 0x2500
    //                {corner top left 0, corner top right 1, horizontal line 2, vertical line 3, corner top left gras 4, corner top right gras 5
    //                 bot left corner 6, bot right corner 7, corner bot left gras 8, corner bot right gras 9, horizontal line gras 10, Vertical line gras 11
    //                 double horizontal line 12, croisement 13 , double line top left corner 14, double line bot left corner 15, double line top right corner 16
    //                 double line bot right corner 17, horizontal double line 18, vertical double line 19, diagonal to right / 20, diagonal to left \21,
    //                 bot right corner robot 22, bot left corner robot 23, top right corner 24, top left corner 25, double line transition to top 26,
    //                 double line transition to bot 27, underscore 28, roue Robot 29}
    char unicode[][4] = {"\u250C", "\u2510", "\u2500", "\u2502", "\u250F", "\u2513", "\u2514", "\u2518", "\u2517", "\u251B", "\u2501", "\u2503", "\u2550", "\u2563",
                         "\u2554", "\u255A", "\u2557", "\u255D", "\u2550", "\u2551", "\u2571", "\u2572", "\u256F", "\u2570", "\u256E", "\u256D", "\u2569", "\u2566", "\uFF3F",
                         "\u25A0"};
    adress = 0x0000;
    char charConverter;
    char carriageReturn = '\r';
    char lineReturn = '\n';
    char text[19] = " Produit par: MASD";
    for (uint8_t i = 0; i < LENGTH_ADDRESS; ++i)
    {
        for (uint8_t j = 0; j < WIDTH_ADDRESS; ++j)
        {
            memory.lecture(adress, &codeMemory);
            if (codeMemory == asciiSpace)
            {
                charConverter = codeMemory;
                uart.transmit(&charConverter, 1);
            }
            else
            {
                uart.transmit(unicode[codeMemory], 4);
            }
            adress++;
        }

        uart.transmit(&carriageReturn, 1);
        uart.transmit(&lineReturn, 1);
    }
    uart.transmit(text, 19);
    uart.transmit(&carriageReturn, 1);
    uart.transmit(&lineReturn, 1);
}

uint8_t calculateDistance(uint8_t data)
{
    uint8_t distance = 0;
    if (data > 74)
    {
        distance = 6;
        return distance;
    }
    else if (data <= 74 && data >60){
        distance = 5;
        return distance;
    }
    else if (data <= 60 && data > 45)
    {
        distance = 4;
            return distance;
    }
    else if (data <= 45 && data > 35)
    {
        distance = 3;
            return distance;
    }
    else if (data <= 35 && data > 23)
    {
        distance = 2;
            return distance;
    }
    else if (data <= 23)
    {
        distance = 1;
            return distance;
    }
    return 255;
}

ISR(INT0_vect)
{
    if (gCurrentState == State::WAIT)
    {
        gCurrentState = State::BUTTON_PRESSED;
    }
    else
    {
        gCurrentState = State::PRINTING_MAP;
    }
}

ISR(TIMER1_COMPA_vect)
{
    if (gfrequence)
    {
        led.setOff();
    }

    else
    {
        led.setGreen();
    }

    gfrequence = !gfrequence;
}

void stopTimer()
{
    TCCR1B &= ~(1 << CS00);
    TCCR1B &= ~(1 << CS01);
    TCCR1B &= ~(1 << CS02);
    TCNT1 = 0;
}

int main()
{
    
    SoundManager sound = SoundManager::getInstance();
    Memoire24CXXX memory;
    ExternalInterrupterManager &interrupterManager = ExternalInterrupterManager::getInstance();
    TimerManager timer = TimerManager::getInstance();
    ReceptionInfra reception = ReceptionInfra::getInstance();
    Uart uart= Uart::getInstance();
    uint8_t distanceFromBehind = 0;
    for (int i =3000; i< 3012; i++){
        memory.ecriture(i,0);
        _delay_ms(MEMORY_DELAY);
    }

    interrupterManager.setInterrupt(ExternalInterrupterManager::Interrupters::Interrupter0, true);
    
    uint16_t compare = 770;
    bool valid;
    while (true)
    {
        switch (gCurrentState)
        {
        case State::INIT:
            findingCenterBox();
            distanceFromBehind=calculateDistance(gInitialDistance);
            drawTemplate();
            gCurrentState=State::WAIT;
            break;

        case State::WAIT:
            led.setRed();
            break;

        case State::BUTTON_PRESSED:
            for (uint8_t i = 0; i < SOUND_ITERATIONS; i++)
            {
                sound.play(NOTE_HIGH_PITCHED_SOUND);
                _delay_ms(SOUND_DELAY);
                sound.stop();
                _delay_ms(SOUND_DELAY);
            }

            gCurrentState = State::RECEIVING_IR;
            break;

        case State::RECEIVING_IR:
            led.setOff();
            valid=!reception.reception();
            if (valid)
            {
                sound.play(NOTE_LOW_PITCHED_SOUND);
                _delay_ms(WAIT_1_SECOND);
                sound.stop();
                gCurrentState = State::WAIT;
            }

            else
            {
                led.setGreen();
                gCurrentState = State::WAIT_MAP;
            }
            break;

        case State::PRINTING_MAP:
            led.setOff();
            

            timer.setTimer(
                TimerManager::Timers::TIMER1,
                TimerManager::Modes::CTC_1_COMPARE,
                TimerManager::Prescalers::SCALE1024,
                compare,
                0);

            addSegmentsToMap();
            drawingRobot1(memory, gInitialPosition, distanceFromBehind );
            displayImage();
         

            gCurrentState = State::WAIT;
            break;

            
        }
    }
    return 0;
}