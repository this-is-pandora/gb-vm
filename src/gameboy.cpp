#include "../include/gameboy.h"

GameBoy::GameBoy()
{
    mmu = new MMU();
    cpu = new CPU(mmu);
    gpu = new GPU(mmu);
    // jp = new JoyPad();
    // spu = new SPU();
}

GameBoy::~GameBoy()
{
    delete cpu;
    delete gpu;
    delete mmu;
}

void GameBoy::update()
{
    const int MAX_CYCLES = 69905;
    int current_cycles = 0;
    // emulation loop
    while (current_cycles < MAX_CYCLES)
    {
        current_cycles += cpu->tick();
        gpu->tick(current_cycles);
    }
    // renderGraphics();
}

void GameBoy::emulate()
{
    if (!unpaused)
    {
        cycles = cpu->tick(); // execute instrctns.
        gpu->tick(cycles);    // step the GPU
        // TODO: handle timers & interrupts out here?
        // TODO: handle window events & controls in VBLANK
        // if (mmu->readByte())
        // handleWindowEvents(event);
    }
}
/*
void GameBoy::game_loop() {
    // halt
    // nop
    // Vblank interrupt
    // clear v-blank flag
    // buttons
    // game operation
    // repeat
} */