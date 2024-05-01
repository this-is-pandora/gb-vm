#include "../include/gameboy.h"
#include <iostream>

GameBoy::GameBoy()
{
    cycles = 0;
    total_cycles = 0;
    unpaused = true;
}

GameBoy::~GameBoy()
{
    delete cpu;
    delete gpu;
    // delete mmu;
}
void GameBoy::loadMemory(char *rom)
{
    mmu->loadROM(rom, 0x8000);
    mmu->loadBootROM();
}
void GameBoy::initialize()
{
    // mmu->loadBootROM();
    mmu = std::make_shared<MMU>();
    cpu = new CPU(mmu);
    gpu = new GPU(mmu);
    h_interrupt = new InterruptHandler(mmu);
    h_timer = new Timer(mmu);
    loadMemory("../bin/Tetris.gb");
    gpu->initGraphics();
}

void GameBoy::emulate()
{
    // TODO: throttle the emulation
    if (total_cycles < MAX_CYCLES)
    {
        cycles = cpu->tick(); // step the CPU
        total_cycles += cycles;
        gpu->tick(cycles); // step the GPU

        if (h_timer->clockEnabled())
            h_timer->handleTimers(cycles, h_interrupt);

        /* TODO: interrupt handler
        if (mmu->interruptsEnabled())
            h_interrupt->handleInterrupts(cpu->getPC(), cpu->getSP()); */

        // TODO: handle window events & controls in VBLANK
    }
    else
        total_cycles = 0;
}
