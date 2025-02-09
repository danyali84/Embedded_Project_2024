// Name: Antoine Beaunoyer, Danya Li, Samer Chebair and Myriam Turki
// Section: 1
// Team: 1923
// Travail: tp9 - SoundManager
// Description: The goal of the program is to emit sounds with diferent frequencies through a class
// Ports Used: D6 is the red wire and D7 is the black wire

#include <avr/io.h>
#include "TimerManager.h"

class SoundManager{
    public:
        static SoundManager& getInstance();
        void play(uint8_t note);
        void stop();
    private:
        SoundManager()= default;
        static TimerManager timer_;
        static SoundManager myInstance_;
};
