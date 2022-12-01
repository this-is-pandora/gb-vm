#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include "interrupts.h"
#include "opcodes.h"

/* 8-bit CPU */
class CPU {
    public:
        struct registers {
            uint8_t A,B,C,D,E,F,H,L = 0;
            uint16_t SP = 0xFFFE;
            uint16_t PC = 0x100;
        } registers;
        uint8_t sram[0x2000]; // 8kB
        uint8_t vram[0x2000];
        int fetch();
        void decode(int opcode);
        void execute(int opcode);
        void tick(); // clock tick
};
#endif