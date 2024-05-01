#pragma once
#include <stdint.h>
#include "memory.h"
#include <memory>

#define IER 0xFF0F // interrupt request/flag register
#define IE 0xFFFF  // interrupt enable register

enum Interrupts
{
    VBLANK,
    LCD,
    TIMER,
    // SERIAL,
    JOYPAD
};

class InterruptHandler
{
private:
    // MMU *mmu;
    std::shared_ptr<MMU> mmu;
    bool readInterrupt(Interrupts i);

public:
    InterruptHandler(std::shared_ptr<MMU> mmu);
    void handleInterrupts(uint16_t &pc, uint16_t &sp);
    void requestInterrupt(int id);
    void serviceInterrupt(int id, uint16_t &pc, uint16_t &sp);
};
