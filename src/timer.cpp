#include "timer.h"

void Timer::tick(int cycles) {
    //incDiv()
    // 
}

//TODO: finish
bool Timer::clockEnabled(uint8_t tmc) {
    // bit 0 & 1: set timer frequency
    // bit 2: turn timer on or off
    uint8_t bits = tmc & 7;
    return ((tmc) & (1 << bits));
}

uint8_t Timer::readClockFrequency(uint8_t tima) {
    return tima & 0x3;
}

void Timer::setClockFrequency(uint8_t tima) {
    uint8_t freq = readClockFrequency(tima);
    switch (freq) {
        case 0: timerCounter = 1024; break; // 4096
        case 1: timerCounter = 16; break; // 262144
        case 2: timerCounter = 64; break; // 65536
        case 3: timerCounter = 256; break; // 16382
    }
}

// increment the divider register
void incDiv(uint8_t* m_rom, uint16_t dr_addr, uint8_t cycles) {
    uint8_t m_dividerRegister = 0;
    uint8_t m_dividerCounter = 0;
    m_dividerRegister += cycles;
    if (m_dividerCounter >= 255) {
        m_dividerCounter = 0;
        m_rom[0xFF04]++; //TODO: fix
    }
}