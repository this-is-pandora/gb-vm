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
    CPU *cpu;
    bool readInterrupt(Interrupts i);

public:
    InterruptHandler(std::shared_ptr<MMU> mmu, CPU *_cpu);
    void handleInterrupts();
    void requestInterrupt(int id);
    void serviceInterrupt(int id);
};
