#include "include/gameboy.h"

void GameBoy::write(uint8_t addr, uint8_t value) {
    if (addr < 0x8000) {
        // read-only memory so do nothing
    }
    else if ((addr >= 0xE000) && (addr < 0xFE00)) {
        // write to echo RAM
    }
    else if ((addr >= 0xFEA0) && (addr < 0xFEFF)) {
        // restricted area
    }
    else {
        //memory->rom[addr] = value;
    }
}

void GameBoy::update() {
    const int MAX_CYCLES = 69905;
    int current_cycles = 0; // current # of cycles
    while (current_cycles < MAX_CYCLES) {
        //uint8_t opcode = cpu->fetch();
        //int cycles = cpu->execute();
    }
}
void GameBoy::game_loop() {
    // halt
    // nop
    // Vblank interrupt
    // clear v-blank flag
    // buttons
    // game operation
    // repeat
}