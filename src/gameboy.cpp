#include "include/gameboy.h"


GameBoy::GameBoy() {
    memory = new MMU();
    timer = new Timer();
    cpu = new CPU(memory);
    // ppu = new PPU();
    // jp = new JoyPad();
    //TODO: etc.
}

void GameBoy::update() {
    const int MAX_CYCLES = 69905;
    int current_cycles = 0;
    // emulation loop
    while (current_cycles < MAX_CYCLES) {
        // int cycles = executeNextOpcode();
        // current_cycles += cycles;
        // updateTimer();
        // updateGraphics();
        // handleInterrupts();
    }
    //renderGraphics();
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