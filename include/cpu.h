#pragma once
#include <stdint.h>
#include <memory>
#include "memory.h"
#include "interrupts.h"
#include "timer.h"

// for doing flag calculations
#define FLAG_MASK_Z 128
#define FLAG_MASK_N 64
#define FLAG_MASK_H 32
#define FLAG_MASK_C 16
#define FLAG_Z 7 // zero
#define FLAG_N 6 // subtract
#define FLAG_H 5 // half carry
#define FLAG_C 4 // carry

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
    friend class InterruptHandler;

private:
    Register AF, BC, DE, HL;
    uint16_t sp; // 0xFFFE
    uint16_t pc; // will be 0x0100 after booting sequence
    // MMU *mmu;                       // memory management unit
    std::shared_ptr<MMU> mmu;

    // 256 opcodes & # of clock cycles each takes to execute
    // TODO: merge these 2 arrays into one?
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
    uint8_t op_ext_cycles[0x100] = {
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8};
    int clocksum; // counts # of clock cycles passed
    int clockSpeed;
    /*TODO: these are CPU states, perhaps put them in an enum later */
    bool accessedMemory;
    bool cpuStopped;
    bool cpuHalted;
    bool pendingEnable, pendingDisable;
    bool debugMode;
    // TODO: move these instructions into an InstructionSet class maybe
    //  CPU control methods
    void CPU_NOP();  // no operation
    void CPU_STOP(); // halt CPU & LCD display until button pressed
    void CPU_CPL();  // complement A
    void CPU_CCF();  // complement carry flag
    void CPU_SCF();  // set carry flag
    void CPU_DAA();  // decimal adjust register A
    void CPU_HALT(); // power down CPU until an interrupt occurs
    void CPU_DI();   // disable interrupts
    void CPU_EI();   // enable interrupts

    // 8-bit loading
    void CPU_REG_LD(uint8_t &reg, uint8_t load);      // register to register
    void CPU_REG_ROM_LD(uint8_t &reg, uint16_t addr); // memory to register loads
    void CPU_ROM_REG_LD(uint16_t addr, uint8_t reg);  // register to memory load
    void CPU_REG_IMM_LD(uint8_t &reg, uint8_t imm);   // immediate into register
    void CPU_8BIT_LD(uint8_t &reg);
    // 8-bit ALU
    void CPU_8BIT_ADD(uint8_t &reg, uint8_t add, bool useImmediate, bool addCarry);
    void CPU_8BIT_SUB(uint8_t &reg, uint8_t sub, bool useImmediate, bool subCarry);
    void CPU_8BIT_INC(uint8_t &reg);
    void CPU_8BIT_MEM_INC(uint16_t addr);
    void CPU_8BIT_DEC(uint8_t &reg);
    void CPU_8BIT_MEM_DEC(uint16_t addr);
    void CPU_8BIT_CP(uint8_t reg, uint8_t cp);
    void CPU_8BIT_AND(uint8_t &reg, uint8_t operand, bool useImmediate);
    void CPU_8BIT_OR(uint8_t &reg, uint8_t operand, bool useImmediate);
    void CPU_8BIT_XOR(uint8_t &reg, uint8_t operand, bool useImmediate);
    void CPU_8BIT_ADC(uint8_t reg);
    // 16-bit ops
    void CPU_16BIT_LD(uint16_t &reg);
    void CPU_16BIT_REG_LD(uint16_t &reg, uint16_t src); // 16-bit register-to-register load
    void CPU_16BIT_ROM_LD(uint16_t addr, uint16_t src); // reg to memory
    void CPU_16BIT_ADD(uint16_t &reg, uint16_t add);
    void CPU_16BIT_SUB(uint16_t &reg, uint16_t sub);
    void CPU_16BIT_INC(uint16_t &reg);
    void CPU_16BIT_DEC(uint16_t &reg);
    void CPU_SBC(uint8_t reg);
    // stack
    void CPU_PUSH(Register reg); // push into stack
    void CPU_POP(Register &reg); // pop stack into reg
    // rotates & shifts
    void CPU_RLC(uint8_t &reg); // shift left w/ bit-7 to bit-0 & stored in c-flag
    void CPU_RL(uint8_t &reg);  // shift left w/ c-flag put in bit-0 & bit-7 in c-flag
    void CPU_RRC(uint8_t &reg); // shift right w/ bit-0 to bit-7 & stored in c-flag
    void CPU_RR(uint8_t &reg);  // shift right w/ c-flag in bit-7 & bit-0 in c-flag
    void CPU_RL_MEM(uint16_t addr);
    void CPU_RR_MEM(uint16_t addr);
    void CPU_RLC_MEM(uint16_t addr);
    void CPU_RRC_MEM(uint16_t addr);
    // shifts
    void CPU_SLA(uint8_t &reg); // shift left, bit-7 in c-flag, bit-0 reset
    void CPU_SRA(uint8_t &reg); // right shift, bit-0 in c-flag, bit-7 stays the same
    void CPU_SRL(uint8_t &reg); // right shift, bit-0 in c-flag, bit-7 reset
    void CPU_SLA_MEM(uint16_t addr);
    void CPU_SRA_MEM(uint16_t addr);
    void CPU_SRL_MEM(uint16_t addr);
    // bits & bytes
    void CPU_BIT(uint8_t n, uint8_t r);  // test bit
    void CPU_SET(uint8_t &n, uint8_t r); // set bit
    void CPU_RES(uint8_t &n, uint8_t r); // reset bit
    void CPU_SWAP(uint8_t &n);
    // jumps
    void CPU_JP(uint16_t addr);
    void CPU_JR(int8_t n);
    // calls
    void CPU_CALL(uint16_t nn);
    // restarts
    void CPU_RST(uint8_t n);
    // returns
    void CPU_RET();
    void CPU_RETI();
    // fetch, decode, execute
    uint8_t fetch();              // read pc & return opcode
    int execute(uint8_t opcode);  // decode opcode & execute
    void executeExtendedOpcode(); // if opcode starts w/ 0xCB

public:
    // CPU(MMU *mmu);
    CPU(std::shared_ptr<MMU> mmu);
    ~CPU();

    void status();
    uint16_t getPC();
    uint16_t getSP();
    uint16_t getOP();
    // flag methods
    bool getZeroFlag() const;
    void setZeroFlag(bool val);
    bool getSubtractFlag() const;
    void setSubtractFlag(bool val);
    bool getCarryFlag() const;
    void setCarryFlag(bool val);
    bool getHalfCarryFlag() const;
    void setHalfCarryFlag(bool val);
    // read memory
    uint8_t readByte(uint16_t addr);
    uint16_t readWord(uint16_t addr);
    // write memory
    void writeWord(uint16_t, uint16_t data);
    void writeByte(uint16_t addr, uint8_t data);
    // save & load
    void saveState();
    void loadState();
    // run the CPU
    int tick();
};
