#include "../include/gameboy.h"

GameBoy::GameBoy()
{
    mmu = new MMU();
    cpu = new CPU();
    gpu = new GPU(mmu);
    // gui = new GUI();
    // ppu = new PPU();
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
        // int cycles = executeNextOpcode();
        // current_cycles += cycles;
        // updateTimer();
        // updateGraphics();
        // handleInterrupts();
        current_cycles += cpu->tick();
        // handleTimers(current_cycles)
        // updateGPU(current_cycles);
        // updateSPU();
        // handleInputs();
        // handleInterrupts();
    }
    // renderGraphics();
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