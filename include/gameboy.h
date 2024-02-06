#pragma once
#include <cstdint>
#include "cpu.h"

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
* PC (program counter) points to the next instruction to be executed
* SP (stack pointer) - stores current stack position
* the maximum ROM size for a game is 32kB, but with MBC's, we can play games of larger sizes

* This is the code for the emulator itself.
* This is where it is all tied together
*/
// the emulator itself
class GameBoy {
    private:
        CPU* cpu;
        MMU* memory;
        Timer* timer;
        //PPU *ppu;
    public:
        GameBoy();
        ~GameBoy();
        void executeInstrctn();
        void handleInput();
        void scanLine();
        void update();
    };