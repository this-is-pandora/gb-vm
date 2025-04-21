#pragma once
#include <cstdint>
#include <memory>
#include <thread>
#include "cpu.h"
#include "memory.h"
#include "interrupts.h"
#include "timer.h"
#include "graphics.h"

#define FPS 60
#define MAX_CYCLES 669905
// #define DEBUG_M
#ifdef DEBUG_M
#include <fstream>
#endif
/*
 * GameBoy has 8 8-bit registers (A,B,C,...,H,L)
 * and 2 16-bit registers (SP, PC)
 * some registers can be combined to create one 16-bit register
 * namely: AF, BC, DE, and HL
 * the F (flag) register stores the results of math operations, e.g.
 * AND, OR, addition, subtraction, etc.
 * structure of the flag register:
 * bit: 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
 * val: Z   N   H   C   0   0   0   0
 * PC (program counter) points to the instruction to be executed
 * SP (stack pointer) - stores current stack position
 * the maximum ROM size for a game is 32kB, but with MBC's,
 * we can play games of larger sizes
 */
// the emulator itself
class GameBoy
{
private:
    std::shared_ptr<MMU> mmu;
    CPU *cpu;
    GPU *gpu; // aka the ppu or "pixel processing unit"
    InterruptHandler *h_interrupt;
    Timer *h_timer;
    int cycles, total_cycles;

    // handling key presses/releases
    uint8_t joypad; // stores the joypad state
    int handleKeyEvent(SDL_Event &e);
    void handleKeyPressed(int key);
    void handleKeyReleased(int key);

public:
    bool unpaused;
    GameBoy();
    ~GameBoy();
    void loadMemory(char *rom);
    void initialize();

    void handleInput(SDL_Event &e);
    void handleInterrupts();
    uint8_t readInput(uint8_t val);

    void emulate();
};