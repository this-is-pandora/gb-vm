#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include "./opcodes.h"

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
*/

class GameBoy {
    public:
        uint8_t A,B,C,D,E,F,H,L = 0;
        uint16_t SP = 0xFFFE;
        uint16_t PC = 0x100;
        uint8_t memory[8000]; // 8kB of memory
    
        bool isPaused = false;
        bool isHalted = false;
        bool usingBootROM = false;

        void initialize();
        void loadGame();
        void saveGame();
        void emulateCycle();
        void fetch();
        void decode();
        void execute();
        
        void run(); // start the emulator
};

#endif