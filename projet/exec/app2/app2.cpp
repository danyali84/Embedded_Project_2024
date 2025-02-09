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
#include "LineSensor.h"
#include "Debug.h"
#include "SoundManager.h"
#include "TransmissionInfra.h"

enum class States
{
    WAITING,
    TRANSFER_INFRA,
    TRANSFER_UART
};

// speeds
const uint8_t TURN_SPEED = 110;
const uint8_t FORWARD_SPEED = 130;
const uint8_t ADJUSTMENT_SPEED = 112;
const uint8_t EXTREME_ADJUSTMENT_SPEED = 103;
const uint8_t MAX_SPEED = 255;

// sound
constexpr uint8_t NOTE_HIGH_PITCHED_SOUND = 79;
constexpr uint8_t NOTE_LOW_PITCHED_SOUND = 45;

// delays
    // memory
constexpr uint8_t MEMORY_DELAY = 10;

    // led
const uint8_t LED_DELAY = 200;

    // turns
const uint16_t TURN_ADJUST_DELAY = 1600;
const uint16_t TURN_DELAY = 900;

    // Line mapping and check
const uint16_t MAPPING_DELAY = 550;
const uint16_t PLACEMENT_MAPPING_DELAY = 500;
const uint16_t CHECK_DELAY = 300;
const uint16_t STOP_DELAY = 1000;

    // others
const uint8_t ADJUSTMENT_BUFFER_DELAY = 1;
const uint8_t WAITING_DELAY = 30;

const uint8_t READ_MEM_DELAY = 5;
const uint8_t SOUND_DELAY = 50;
const uint16_t IR_DELAY = 2000;

// segment length
const uint16_t MAX_LEN_1 = 1300;
const uint16_t MAX_LEN_2 = 2000;

// Miscellaneous values
volatile States gState = States::WAITING;
const uint8_t N_DATA = 12;
const uint16_t START_ADDRESS = 3000;

const uint8_t WIDTH_ADDRESS = 102;
const uint8_t LENGTH_ADDRESS = 26;
constexpr uint8_t SOUND_ITERATIONS = 3;

volatile bool gFrequence = true;


ISR(INT0_vect)
{
    if (gState != States::TRANSFER_UART)
    {
        gState = States::TRANSFER_INFRA;
    }
}

ISR(INT2_vect)
{
    if (gState != States::TRANSFER_INFRA)
    {
        gState = States::TRANSFER_UART;
    }
}

ISR(TIMER1_COMPA_vect)
{
    LedManager led = LedManager::getInstance();
    if (gFrequence)
    {
        led.setOff();
    }
    else
    {
        led.setGreen();
    }
    gFrequence = !gFrequence;
}

void initialRoutine()
{
    LedManager led = LedManager::getInstance();

    led.setGreen();
    _delay_ms(LED_DELAY);
    led.setOff();
    _delay_ms(LED_DELAY);

    led.setGreen();
    _delay_ms(LED_DELAY);
    led.setOff();
    _delay_ms(LED_DELAY);

    led.setRed();
    _delay_ms(LED_DELAY);
    led.setOff();
    _delay_ms(LED_DELAY);
}

void adjustForwardSpeed()
{
    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();
    if (line.sensorFarRight())
    {
        motor.setLeftSpeed(FORWARD_SPEED);
        motor.setRightSpeed(EXTREME_ADJUSTMENT_SPEED);
    }
    else if (line.sensorShortRight())
    {
        motor.setLeftSpeed(FORWARD_SPEED);
        motor.setRightSpeed(ADJUSTMENT_SPEED);
    }
    if (line.sensorFarLeft())
    {
        motor.setLeftSpeed(EXTREME_ADJUSTMENT_SPEED);
        motor.setRightSpeed(FORWARD_SPEED);
    }
    else if (line.sensorShortLeft())
    {
        motor.setLeftSpeed(ADJUSTMENT_SPEED);
        motor.setRightSpeed(FORWARD_SPEED);
    }
}

void adjustBackwardsSpeed()
{
    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();
    if (line.sensorShortLeft() || line.sensorFarLeft())
    {
        motor.setRightSpeed(ADJUSTMENT_SPEED);
        motor.setLeftSpeed(FORWARD_SPEED);
    }
    if (line.sensorShortRight() || line.sensorFarRight())
    {
        motor.setRightSpeed(FORWARD_SPEED);
        motor.setLeftSpeed(ADJUSTMENT_SPEED);
    }
}

void passShortSides()
{
    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();

    motor.setLeftSpeed(FORWARD_SPEED);
    motor.setRightSpeed(FORWARD_SPEED);

    _delay_ms(MAPPING_DELAY);

    while (!(line.sensorMid() && line.sensorFarRight()))
    {
        adjustForwardSpeed();
    }

    motor.reset();
}

bool turnCorner(bool isLeftTurn)
{
    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();

    _delay_ms(STOP_DELAY);

    // adjust robot placement

    motor.setLeftSpeed(FORWARD_SPEED);
    motor.setRightSpeed(FORWARD_SPEED);
    _delay_ms(TURN_ADJUST_DELAY);

    motor.reset();
    _delay_ms(TURN_ADJUST_DELAY);

    // turn direction
    motor.setLeftDirection(!isLeftTurn);
    motor.setRightDirection(isLeftTurn);

    // turning
    motor.setRightSpeed(MAX_SPEED);
    motor.setLeftSpeed(MAX_SPEED);
    _delay_ms(WAITING_DELAY);

    motor.setRightSpeed(TURN_SPEED);
    motor.setLeftSpeed(TURN_SPEED);
    _delay_ms(TURN_DELAY);

    uint16_t counter = 0;

    while (!(line.sensorFarLeft() || line.sensorShortRight()))
    {
        counter++;
        _delay_ms(ADJUSTMENT_BUFFER_DELAY);
    }

    motor.reset();
    _delay_ms(TURN_ADJUST_DELAY);

    return counter > MAX_LEN_1;
}

bool checkLine()
{
    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();

    bool shortLine = turnCorner(false);
    if (shortLine)
        return shortLine;

    motor.setLeftDirection(true);
    motor.setRightDirection(true);
    motor.setLeftSpeed(FORWARD_SPEED);
    motor.setRightSpeed(FORWARD_SPEED);

    while (!(line.sensorMid() && line.sensorShortRight() && line.sensorFarRight()))
    {
        adjustBackwardsSpeed();
    }

    motor.reset();
    _delay_ms(STOP_DELAY);
    if (line.sensorShortLeft()){
        motor.setLeftSpeed(EXTREME_ADJUSTMENT_SPEED);
        motor.setRightSpeed(FORWARD_SPEED);
    }else{
        motor.setLeftSpeed(FORWARD_SPEED);
        motor.setRightSpeed(FORWARD_SPEED);
    }

    _delay_ms(CHECK_DELAY);
    motor.reset();
    _delay_ms(STOP_DELAY);
    return shortLine;
}

uint8_t mapLine()
{
    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();
    uint8_t len = 0;

    uint16_t counter = 0;

    motor.setLeftSpeed(FORWARD_SPEED);
    motor.setRightSpeed(FORWARD_SPEED);
    _delay_ms(PLACEMENT_MAPPING_DELAY);

    while (line.sensorMid() || line.sensorShortRight() || line.sensorShortLeft() || line.sensorFarLeft() || line.sensorFarRight())
    {
        adjustForwardSpeed();
        counter++;
        _delay_ms(ADJUSTMENT_BUFFER_DELAY);
    }

    motor.reset();
    _delay_ms(STOP_DELAY);
    if (counter <= MAX_LEN_1)
    {
        len = 1;
    }

    else if (counter <= MAX_LEN_2)
    {
        len = 2;
    }

    else
    {
        len = 3;
    }

    motor.setLeftDirection(true);
    motor.setRightDirection(true);
    motor.setRightSpeed(FORWARD_SPEED);
    motor.setLeftSpeed(FORWARD_SPEED);

    while (!line.all())
    {
        adjustBackwardsSpeed();
    }

    motor.reset();
    _delay_ms(STOP_DELAY);

    motor.setLeftSpeed(FORWARD_SPEED);
    motor.setRightSpeed(FORWARD_SPEED);
    _delay_ms(MAPPING_DELAY);

    motor.reset();
    _delay_ms(STOP_DELAY);

    turnCorner(true);
    return len;
}

void passLongSides(bool secondLine)
{

    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();
    uint16_t counter = 0;
    uint8_t nSegment = 0;
    uint16_t segmentPlacement[6];
    uint8_t segmentLength[6];

    motor.setLeftSpeed(FORWARD_SPEED);
    motor.setRightSpeed(FORWARD_SPEED);

    while (true)
    {
        counter++;
        if (line.sensorMid() && line.sensorShortRight() && line.sensorFarRight())
        {

            motor.reset();
            bool shortLine = checkLine();

            if (shortLine)
            {
                turnCorner(false);

                motor.setLeftSpeed(FORWARD_SPEED);
                motor.setRightSpeed(FORWARD_SPEED);
                _delay_ms(CHECK_DELAY);

                segmentLength[nSegment] = 1;
                segmentPlacement[nSegment] = counter;
                nSegment++;
            }

            else if (!(line.sensorFarLeft() && line.sensorShortLeft()))
            {
                motor.setLeftSpeed(FORWARD_SPEED);
                motor.setRightSpeed(FORWARD_SPEED);
                _delay_ms(MAPPING_DELAY);
                break;
            }

            else
            {
                segmentLength[nSegment] = mapLine();
                segmentPlacement[nSegment] = counter;
                nSegment++;

                motor.setLeftSpeed(FORWARD_SPEED);
                motor.setRightSpeed(FORWARD_SPEED);
            }
        }

        adjustForwardSpeed();
        _delay_ms(ADJUSTMENT_BUFFER_DELAY);
    }

    motor.reset();

    uint16_t adressModifier = secondLine ? START_ADDRESS + (N_DATA / 2) : START_ADDRESS;
    uint16_t counterPerChar = counter / (50);


    Memoire24CXXX memory;
    
    for (uint16_t i = 0; i < nSegment; i++)
    {
        uint8_t position= segmentPlacement[i]/counterPerChar;
        position= secondLine ? position+50 : position;

        memory.ecriture(adressModifier + i, position);
        _delay_ms(READ_MEM_DELAY);
        adressModifier++;
        memory.ecriture(adressModifier + i, segmentLength[i]);
        _delay_ms(READ_MEM_DELAY);
    }
}

void endMapping()
{
    MotorManager motor = MotorManager::getInstance();
    LineSensor line = LineSensor::getInstance();

    motor.setLeftSpeed(FORWARD_SPEED);
    motor.setRightSpeed(FORWARD_SPEED);

    // wait for line on left
    while (!(line.sensorMid() && line.sensorFarLeft()))
    {
        adjustForwardSpeed();
    }

    motor.reset();

    turnCorner(true);

    // go to end of line
    while (line.sensorMid() || line.sensorShortLeft() || line.sensorShortRight())
    {
        adjustForwardSpeed();
    }

    motor.reset();
}

void transferIR()
{
    cli();
    TransmissionInfra infra = TransmissionInfra::getInstance();

    Memoire24CXXX memory;
    uint8_t values[N_DATA];
    uint8_t value;
    for (uint16_t i = START_ADDRESS; i < START_ADDRESS+N_DATA; i++)
    {
        memory.lecture(i, &value);
        values[i]=value;
        _delay_ms(READ_MEM_DELAY);
    }
    infra.transmit(values);
    sei();
}


void writeInMemory(uint8_t character, uint16_t address, Memoire24CXXX memory)
{

    memory.ecriture(address, character);
    _delay_us(MEMORY_DELAY);
}

uint16_t calculateAddress(uint8_t posX, uint8_t posY)
{
    uint16_t address;
    address = posX * WIDTH_ADDRESS + posY;
    return address;
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
void drawTemplate()

{
    Memoire24CXXX memory;
    uint16_t address = 0x0000;
    uint8_t input = 0x00;
    uint8_t asciiSpace = 0x20;
    for (uint8_t i = 0; i < LENGTH_ADDRESS; ++i)
    {
        for (uint8_t j = 0; j < WIDTH_ADDRESS; ++j)
        {
            memory.ecriture(address, asciiSpace);
            address++;
            _delay_us(MEMORY_DELAY);
        }
    }

    uint16_t tempAddress = 0x0000;
    // top left corner
    tempAddress = calculateAddress(0, 0);
    input = 0;
    writeInMemory(input, tempAddress, memory);

    // top right corner
    tempAddress = calculateAddress(0, 101);
    input = 1;
    writeInMemory(input, tempAddress, memory);

    // bot letft corner
    tempAddress = calculateAddress(25, 0);
    input = 6;
    memory.ecriture(tempAddress, input);
    _delay_us(MEMORY_DELAY);

    // bot right corner
    tempAddress = calculateAddress(25, 101);
    input = 7;
    memory.ecriture(tempAddress, input);
    _delay_us(MEMORY_DELAY);

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
    tempAddress = calculateAddress(3, 7);
    input = 4;
    writeInMemory(input, tempAddress, memory);

    // box corner top right
    tempAddress = calculateAddress(3, 33);
    input = 5;
    writeInMemory(input, tempAddress, memory);

    // box corner bot left
    tempAddress = calculateAddress(22, 7);
    input = 8;
    writeInMemory(input, tempAddress, memory);

    // box corner bot right
    tempAddress = calculateAddress(22, 33);
    input = 9;
    writeInMemory(input, tempAddress, memory);

    // path begin
    input = 12;
    writeHorizontalLine(memory, 13, 37, 41, input);

    // path instersection
    tempAddress = calculateAddress(13, 42);
    input = 13;
    writeInMemory(input, tempAddress, memory);

    // path top left corner
    tempAddress = calculateAddress(5, 42);
    input = 14;
    writeInMemory(input, tempAddress, memory);

    // path bot left corner
    tempAddress = calculateAddress(20, 42);
    input = 15;
    writeInMemory(input, tempAddress, memory);

    // path top right corner
    tempAddress = calculateAddress(5, 93);
    input = 16;
    writeInMemory(input, tempAddress, memory);

    // path bot right corner
    tempAddress = calculateAddress(20, 93);
    input = 17;
    writeInMemory(input, tempAddress, memory);

    // horizontal line for path
    input = 18;
    writeHorizontalLine(memory, 5, 43, 92, input);

    writeHorizontalLine(memory, 20, 43, 92, input);

    // vertical line for path
    input = 19;
    writeVerticalLine(memory, 42, 6, 19, input);

    writeVerticalLine(memory, 93, 6, 19, input);
}

void displayImage()
{
    Memoire24CXXX memory;
    uint8_t asciiSpace = 0x20;
    Uart &uart = Uart::getInstance();
    uint16_t address = 0x0000;
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
    address = 0x0000;
    char charConverter;
    char carriageReturn = '\r';
    char lineReturn = '\n';

    for (uint8_t i = 0; i < LENGTH_ADDRESS; ++i)
    {
        for (uint8_t j = 0; j < WIDTH_ADDRESS; ++j)
        {
            memory.lecture(address, &codeMemory);
            if (codeMemory == asciiSpace)
            {
                charConverter = codeMemory;
                uart.transmit(&charConverter, 1);
            }
            else
            {
                uart.transmit(unicode[codeMemory], 4);
            }
            address++;
        }

        uart.transmit(&carriageReturn, 1);
        uart.transmit(&lineReturn, 1);
    }
    uart.transmit(&carriageReturn, 1);
    uart.transmit(&lineReturn, 1);
}

void stopTimer()
{
    TCCR1B &= ~(1 << CS00);
    TCCR1B &= ~(1 << CS01);
    TCCR1B &= ~(1 << CS02);
    TCNT1 = 0;
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
    
    for (uint16_t address=3000; address<3000+N_DATA; address++){
        memoire.lecture(address,&position);
        address++;
        memoire.lecture(address,&length);
        addLineToMap(position, length);
    }   
}

int main()
{
    initialRoutine();
    Memoire24CXXX mem;
    ExternalInterrupterManager interrupt = ExternalInterrupterManager::getInstance();
    LedManager led = LedManager::getInstance();
    SoundManager sound = SoundManager::getInstance();
    TimerManager timer = TimerManager::getInstance();

    bool secondSide = false;

    for (int i =3000; i< 3012; i++){
        mem.ecriture(i,0);
        _delay_ms(MEMORY_DELAY);
    }

    for (int i = 0; i < 2; i++)
    {
        passShortSides();
        turnCorner(false);

        passLongSides(secondSide);
        secondSide = !secondSide;
    }

    endMapping();

    interrupt.setInterrupt(ExternalInterrupterManager::Interrupters::Interrupter0, true);
    interrupt.setInterrupt(ExternalInterrupterManager::Interrupters::Interrupter2, true);

    uint16_t compare = 770;

    while (true)
    {
        switch (gState)
        {
        case States::WAITING:
            led.setRed();
            _delay_ms(WAITING_DELAY);
            break;

        case States::TRANSFER_INFRA:
            for (uint8_t i = 0; i < SOUND_ITERATIONS; i++)
            {
                sound.play(NOTE_HIGH_PITCHED_SOUND);
                _delay_ms(SOUND_DELAY);
                sound.stop();
                _delay_ms(SOUND_DELAY);
            }

            led.setOff();
            _delay_ms(IR_DELAY);
            transferIR();

            gState = States::WAITING;
            break;

        case States::TRANSFER_UART:
            led.setOff();

            timer.setTimer(
                TimerManager::Timers::TIMER1,
                TimerManager::Modes::CTC_1_COMPARE,
                TimerManager::Prescalers::SCALE1024,
                compare,
                0);

            drawTemplate();
            addSegmentsToMap();
            displayImage();
            stopTimer();

            led.setOff();

            //transferUart();
            gState = States::WAITING;
            break;
        }
    }

    return 0;
}
