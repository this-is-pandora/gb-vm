#pragma once
#include <stdint.h>
#include "./cpu.h"
#include "./memory.h"
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
    std::shared_ptr<MMU> mmu;
    bool readInterrupt(Interrupts i);
public:
    InterruptHandler(std::shared_ptr<MMU> mmu);
    void handleInterrupts(CPU *cpu);
    void requestInterrupt(int id);
    void serviceInterrupt(int id, CPU *cpu);
};
