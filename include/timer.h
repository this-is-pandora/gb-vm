#pragma once
#include <stdint.h>
#include "memory.h";

// memory addresses
#define DIV_REG 0xFF04 // divider register
#define TIMA 0xFF05    // the timer counter
#define TMA 0xFF06     // timer modulo
#define TMC 0xFF07     // timer control

// 1 machine cycle = 4 clock cycles
// clock speed = 4.19MHz

// the timer controller (TMA) has various selectable frequencies
// for the timer (TIMA) to count up to
// these are: 4096Hz, 262144Hz, 65536Hz, 16384Hz

// the timer control (TMC) is a 3-bit register
// where the bits 0 & 1 specify which frequency
// the timer should increment at
// bit 2 specifies whether to run or stop the timer

#define CLOCKSPEED 4194304

class Timer
{
private:
    // TMA frequencies
    int frequencies[4] = {4096, 262144, 65536, 16384};
    int frequency = frequencies[0]; // 4096

    // timer counter is 1024 by default
    // 4194304/4096 = 1024
    int timerCounter = CLOCKSPEED / frequency;
    bool timerEnabled;
    MMU *memory;

public:
    Timer(MMU *memory = NULL);
    void updateTimers(int cycles);
    bool clockEnabled(uint8_t tmc);
    uint8_t readClockFrequency(uint8_t tima);
    void setClockFrequency(uint8_t tima);
};