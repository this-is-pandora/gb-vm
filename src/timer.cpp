#include "../include/timer.h"

Timer::Timer(std::shared_ptr<MMU> memory) : memory(memory)
{
    // memory = memory;
}

void Timer::handleTimers(int cycles, InterruptHandler *interruptHandler)
{
    // memory->incDIV();
    handleDivider(cycles);
    if (timerEnabled)
    {
        t_clocksum += cycles;
        setClockFrequency();
        while (t_clocksum >= (4194304 / frequency))
        {
            memory->incAddr(TIMA);
            // if overflow, do timer interrupt
            // & reset timer to modulo
            if (memory->readByte(TIMA) >= 0xFF)
            {
                // request Timer interrupt
                interruptHandler->requestInterrupt(TIMER);
                // set timer to modulo
                memory->writeByte(TIMA, memory->readByte(TMA));
            }
            t_clocksum -= (4194304 / frequency);
        }
    }
}

// TODO: finish
bool Timer::clockEnabled()
{
    // bit 0 & 1: set timer frequency
    // bit 2: turn timer on or off
    uint8_t n = memory->readByte(TMC);
    uint8_t bits = n & 7;
    return ((n) & (1 << bits));
}

uint8_t Timer::readClockFrequency()
{
    uint8_t n = memory->readByte(TIMA);
    return n & 0x3;
}

void Timer::setClockFrequency()
{
    // uint8_t n = memory->readByte(TIMA);
    uint8_t freq = readClockFrequency();
    switch (freq)
    {
    case 0:
        frequency = frequencies[0];
        break; // 4.19MHZ / 4096 / 1024
    case 1:
        frequency = frequencies[1];
        break; // 262144 / 16
    case 2:
        frequency = frequencies[2];
        break; // 65536 /
    case 3:
        frequency = frequencies[3];
        break; // 16382 / 256
    }
}

void Timer::handleDivider(int cycles)
{
    div_clocksum += cycles;
    if (div_clocksum >= 256)
    {
        div_clocksum -= 256;
        memory->incAddr(DIV_REG);
    }
}