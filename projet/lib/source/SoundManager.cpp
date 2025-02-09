#include "SoundManager.h"
#include "TimerManager.h"

const uint8_t DATA_TO_INDEX = 45;
const uint8_t MAX_INDEX = 36;
const uint8_t FREQUENCY[] = {141, 133, 126, 118, 112, 105, 99, 94, 88, 83, 
79, 74, 70, 66, 62, 59, 55, 52, 49, 46, 44, 41, 39, 37, 35, 33, 31, 29, 27, 
26, 24, 23, 21, 20, 19, 18, 17};

SoundManager SoundManager::myInstance_=SoundManager();

SoundManager& SoundManager::getInstance()
{
    return myInstance_;
}
TimerManager SoundManager::timer_= TimerManager::getInstance();



void SoundManager::play(uint8_t note){
    uint8_t index = note - DATA_TO_INDEX;
    if (index <= MAX_INDEX){
        uint8_t compare = FREQUENCY[index];
        timer_.setTimer(
            TimerManager::Timers::TIMER2, 
            TimerManager::Modes::SOUND, 
            TimerManager::Prescalers::SCALE256, 
            compare,
            0
        );
    }
}

void SoundManager::stop(){
    TCCR2B &= ~(1 << CS20) & ~(1 << CS21) & ~(1 << CS22);
    TCNT2 = 0; 
}