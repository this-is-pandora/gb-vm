#pragma once
#include <stdint.h>
#include "memory.h"
#include <memory>
#include "cpu.h"

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
    std::shared_ptr<MMU> mmu;

public:
    InterruptHandler(std::shared_ptr<MMU> mmu);
    bool readInterrupt(Interrupts i);
    void handleInterrupts();
    void requestInterrupt(int id);
    uint8_t serviceInterrupt(int id);
};
