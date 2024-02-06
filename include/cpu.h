#pragma once
#include <stdint.h>
#include <vector>
#include "interrupts.h"
#include "timer.h"
#include "memory.h"

using namespace std;
// for doing flag calculations
#define FLAG_MASK_Z 128
#define FLAG_MASK_N 64
#define FLAG_MASK_H 32
#define FLAG_MASK_C 16
#define FLAG_Z 7 // zero
#define FLAG_N 6 // subtract
#define FLAG_H 5 // half carry
#define FLAG_C 4 // carry
/*
struct Clock {
    uint8_t t_cycles = 0;
    uint8_t m_cycles = 0; // machine cycles
}; */

union Register
{
    uint16_t value;
    struct
    {
        uint8_t lo; // least significant
        uint8_t hi; // most significant
    };
};

// clock speed ~= 4.19MHz
// only 1 step gets executed every 4 ticks
// 4 clock cycles = 1 machine cycles
// clock speed in machine cycles ~= 1.05 MHz
class CPU
{
private:
    Register AF, BC, DE, HL;
    uint16_t sp = 0xFFFE;
    uint16_t pc = 0x0100;
    MMU *memory;

    // 256 opcodes & # of clock cycles each takes to execute
    uint8_t op_cycles[0x100] = {
        4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4,
        4, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,
        8, 12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,
        8, 12, 8, 8, 12, 12, 12, 4, 8, 8, 8, 8, 4, 4, 8, 4,
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
        8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
        8, 12, 12, 12, 12, 16, 8, 32, 8, 8, 12, 8, 12, 12, 8, 32,
        8, 12, 12, 0, 12, 16, 8, 32, 8, 8, 12, 0, 12, 0, 8, 32,
        12, 12, 8, 0, 0, 16, 8, 32, 16, 4, 16, 0, 0, 0, 8, 32,
        12, 12, 8, 4, 0, 16, 8, 32, 12, 8, 16, 4, 0, 0, 8, 32};
    // Clock clock;
    int clockCycles; // our clock cycle counter
    int clockSpeed;

    bool accessedMemory;
    bool cpuStopped;
    bool cpuHalted;
    bool interruptsEnabled; // ime
    bool debugMode;

    // CPU control methods
    void CPU_NOP(uint8_t cycles); // no operation
    int CPU_STOP();               // halt CPU & LCD display until button pressed
    int CPU_CPL();                // complement A
    int CPU_CCF();                // complement carry flag
    int CPU_SCF();                // set carry flag
    int CPU_DAA();                // decimal adjust register A
    int CPU_HALT();               // power down CPU until an interrupt occurs
    int CPU_DI();                 // disable interrupts
    int CPU_EI();                 // enable interrupts

    // 8-bit data manipulation
    void CPU_REG_LD(uint8_t &reg, uint8_t load, uint8_t cycles);      // register to register
    void CPU_REG_ROM_LD(uint8_t &reg, uint16_t addr, uint8_t cycles); // memory to register loads
    void CPU_ROM_REG_LD(uint16_t addr, uint8_t reg, uint8_t cycles);  // register to memory load
    void CPU_REG_IMM_LD(uint8_t &reg, uint8_t imm, uint8_t cycles);   // immediate into register
    // 8-bit
    void CPU_8BIT_LD(uint8_t &reg, uint8_t cycles);
    void CPU_8BIT_ADD(uint8_t &reg, uint8_t add, uint8_t cycles, bool useImmediate, bool addCarry);
    void CPU_8BIT_SUB(uint8_t &reg, uint8_t sub, uint8_t cycles, bool useImmediate, bool subCarry);
    void CPU_8BIT_INC(uint8_t &reg, uint8_t cycles);
    void CPU_8BIT_MEM_INC(uint16_t addr, uint8_t cycles);
    void CPU_8BIT_DEC(uint8_t &reg, uint8_t cycles);
    void CPU_8BIT_MEM_DEC(uint16_t addr, uint8_t cycles);
    void CPU_8BIT_CP(uint8_t &reg, uint8_t cp, uint8_t cycles);
    void CPU_8BIT_AND(uint8_t &reg, uint8_t operand, uint8_t cycles, bool useImmediate);
    void CPU_8BIT_OR(uint8_t &reg, uint8_t operand, uint8_t cycles, bool useImmediate);
    void CPU_8BIT_XOR(uint8_t &reg, uint8_t operand, uint8_t cycles, bool useImmediate);
    void CPU_8BIT_ADC(uint8_t reg, uint8_t cycles);
    // 16-bit
    void CPU_16BIT_LD(uint16_t &reg, uint8_t cycles);
    void CPU_16BIT_REG_LD(uint16_t &reg, uint16_t src, uint8_t cycles); // 16-bit register-to-register load
    void CPU_16BIT_ROM_LD(uint16_t addr, uint16_t src, uint8_t cycles); // reg to memory
    void CPU_16BIT_ADD(uint16_t &reg, uint16_t add, uint8_t cycles);
    void CPU_16BIT_SUB(uint16_t &reg, uint16_t sub, uint8_t cycles);
    void CPU_16BIT_INC(uint16_t &reg, uint8_t cycles);
    void CPU_16BIT_DEC(uint16_t &reg, uint8_t cycles);
    void CPU_SBC(uint8_t reg, uint8_t cycles);
    // stack
    void CPU_PUSH(Register reg, uint8_t cycles); // push into stack
    void CPU_POP(Register &reg, uint8_t cycles); // pop stack into reg
    // rotates & shifts
    void CPU_RLA();
    void CPU_RRCA();
    void CPU_RRA();
    void CPU_RLCA();
    void CPU_RLC();
    void CPU_RL();
    void CPU_RRC();
    void CPU_RR();
    void CPU_SLA();
    void CPU_SRA();
    void CPU_SRL();
    // bit opcodes
    void CPU_BIT(uint8_t &n, uint8_t r, int cycles); // test bit
    void CPU_SET(uint8_t &n, uint8_t r, int cycles); // set bit
    void CPU_RES(uint8_t &n, uint8_t r, int cycles); // reset bit
    // jumps
    void CPU_JP(uint16_t addr);
    void CPU_JR(int8_t n);
    // calls
    void CPU_CALL(uint16_t nn);
    // restarts
    void CPU_RST(uint8_t n, int cycles);
    // returns
    void CPU_RET();
    void CPU_RETI();
    // fetch, decode, execute
    uint8_t fetch();              // read pc & return opcode
    int execute(uint8_t opcode);  // decode opcode & execute
    void executeExtendedOpcode(); // if opcode starts w/ 0xCB

public:
    CPU(MMU *mmu);
    ~CPU();

    // flag methods
    bool getZeroFlag() const;
    void setZeroFlag();
    bool getSubtractFlag() const;
    void setSubtractFlag();
    bool getCarryFlag() const;
    void setCarryFlag();
    bool getHalfCarryFlag() const;
    void setHalfCarryFlag();
    void setFlags(bool flag_z, bool flag_n, bool flag_h, bool flag_c);

    // read memory
    uint8_t read(uint16_t addr);
    uint8_t readByte(uint16_t addr);
    uint16_t readWord(uint16_t addr);
    // write memory
    void write(uint16_t addr);
    void writeWord(uint16_t, uint16_t data); // write to memory
    void writeByte(uint16_t addr, uint8_t data);
    // check for & handle interrupts
    bool checkInterrupts();
    int handleInterrupts();
    // save & load
    void saveState();
    void loadState();
    // run the CPU
    int tick();
};
