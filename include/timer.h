#pragma once
#include <stdint.h>

// memory addresses
#define dividerRegister 0xFF04 // divider
#define TIMA 0xFF05 // the timer counter
#define TMA 0xFF06 // timer modulo
#define TMC 0xFF07 // timer control

// 1 machine cycle = 4 clock cycles
// clock speed = 4.19MHz
// the timer controller has various selectable frequencies
// for the timer (TIMA) to count up to
// these are: 4096Hz, 262144Hz, 65536Hz, 16384Hz

// the timer control is a 3-bit register 
// where the bits 0 & 1 specify which frequency
// the timer should increment at
// bit 2 specifies whether to run or stop the timer

#define CLOCKSPEED 4194304

class Timer {
    private:
        int frequencies[4] = {4096, 262144, 65536, 16384};
        int frequency;
        int timerCounter = CLOCKSPEED/frequency; // 1024
        CPU *cpu;
    public:
        Timer(int frequency, CPU& cpu);
        void update();
        bool clockEnabled();
        uint8_t readClockFrequency();
        void setClockFrequency();
};