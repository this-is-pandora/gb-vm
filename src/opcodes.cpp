#include "../include/cpu.h"

// cpu control instructions
void CPU::CPU_NOP(uint8_t cycles)
{
    pc++;
    clockCycles += cycles;
}

int CPU::CPU_STOP()
{
    cpuStopped = true;
}

int CPU::CPU_CPL()
{
    AF.hi = (~AF.hi) & 0xFF;
    uint8_t flag = 0x60;
    AF.lo &= 0x90;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF;
    // return 4
}
int CPU::CPU_CCF()
{
    uint8_t flag = (AF.lo & 0x10) ^ 0x10;
    AF.lo &= 0x80;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF;
    // return 4
}
int CPU::CPU_SCF()
{
    uint8_t flag = 0x10;
    AF.lo &= 0x80;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF;
    // return 4
}

// TODO: finish
int CPU::CPU_DAA()
{
    uint8_t A = AF.hi;
    AF.lo = 0;
    // 0 flag
    if (A == 0)
        CPU_SET(AF.lo, FLAG_Z, 0);
    // carry flag
    if (AF.hi > 0xFF)
        CPU_SET(AF.lo, FLAG_C, 0);
}
int CPU::CPU_HALT()
{
    cpuHalted = true;
    // clockCycles += 4;
}
int CPU::CPU_DI()
{
    interruptsEnabled = false;
    // clockCycles += 4;
}
int CPU::CPU_EI()
{
    interruptsEnabled = true;
    // clockCycles += 4
}

// CPU LOAD instructions
// TODO: fix register to register
void CPU::CPU_REG_LD(uint8_t &reg, uint8_t load, uint8_t cycles)
{
    reg = load;
    clockCycles += cycles;
}

// memory to register
void CPU::CPU_REG_ROM_LD(uint8_t &reg, uint16_t addr, uint8_t cycles)
{
    reg = memory->readByte(addr);
    clockCycles += cycles;
}

// register to memory
void CPU::CPU_ROM_REG_LD(uint16_t addr, uint8_t reg, uint8_t cycles)
{
    memory->writeByte(addr, reg);
    clockCycles += cycles;
}
// immediate to memory
void CPU::CPU_8BIT_LD(uint8_t &reg, uint8_t cycles)
{
    uint8_t value = memory->readByte(pc);
    pc++;
    reg = value;
    clockCycles += cycles;
}

// 8-bit arithmetic
void CPU::CPU_8BIT_ADD(uint8_t &reg, uint8_t add, uint8_t cycles, bool useImmediate, bool addCarry)
{
    // TODO
    if (getCarryFlag())
        add += 1;

    AF.lo = 0;
    // zero flag
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles);
    // subtract flag = 0
    CPU_RES(AF.lo, FLAG_N, cycles);

    // half carry
    uint16_t test = (reg & 0xF); // get the lower half nibble
    test += (add & 0xF);         // lower half nibble of add
    if (test > 0xF)              // if overflow, set the half carry flag
        CPU_SET(AF.lo, FLAG_H, cycles);

    // carry flag
    if ((reg + add) > 0xFF)
        CPU_SET(AF.lo, FLAG_C, cycles);

    reg += add;
    // clockCycles += cycles;
}

// TODO: same as above
void CPU::CPU_8BIT_SUB(uint8_t &reg, uint8_t sub, uint8_t cycles, bool useImmediate, bool subCarry)
{
    // setFlags(true, true, true, true);

    AF.lo = 0;
    // 0 flag
    if ((reg - sub) == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles);
    // subtract flag
    CPU_SET(AF.lo, FLAG_N, cycles);
    // carry flag
    if (reg < sub)
        CPU_SET(AF.lo, FLAG_C, cycles);
    // half carry flag
    uint16_t test = (reg & 0xF);
    test -= (sub & 0xF);
    if (test < 0)
        CPU_SET(AF.lo, FLAG_H, cycles);

    reg -= sub;
    clockCycles += cycles;
}
// TODO
void CPU::CPU_8BIT_INC(uint8_t &reg, uint8_t cycles)
{
    AF.lo = 0;
    // zero flag
    if ((reg + 1) == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles); // AF.lo, FLAG_Z
    // subtract flag
    CPU_RES(AF.lo, FLAG_N, cycles);

    // half carry flag
    if ((reg & 0xF) >= 0xF)
        CPU_SET(AF.lo, FLAG_H, cycles);

    reg++;
    clockCycles += cycles;
}

void CPU::CPU_8BIT_ADC(uint8_t reg, uint8_t cycles)
{
    CPU::CPU_8BIT_ADD(AF.hi, reg, cycles, false, true);
}

void CPU::CPU_SBC(uint8_t reg, uint8_t cycles)
{
    CPU_8BIT_SUB(AF.hi, reg, cycles, false, true);
}

void CPU::CPU_8BIT_CP(uint8_t &reg, uint8_t cp, uint8_t cycles)
{
    uint8_t val = reg - cp;
    AF.lo = 0;
    // 0 flag
    if (val == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles);
    // subtract flag
    CPU_SET(AF.lo, FLAG_N, cycles);
    // carry flag
    if (reg < cp)
        CPU_SET(AF.lo, FLAG_C, cycles);
    // TODO: half carry
    uint8_t test = reg & 0xF;
    test -= (cp & 0xF);
    if (test < 0)
        CPU_SET(AF.lo, FLAG_H, cycles);
}
// 16-bit arithmetic
void CPU::CPU_16BIT_ADD(uint16_t &reg, uint16_t add, uint8_t cycles)
{
    reg += add;
    clockCycles += cycles;
}

void CPU::CPU_16BIT_SUB(uint16_t &reg, uint16_t sub, uint8_t cycles)
{
    reg -= sub;
    clockCycles += cycles;
}

void CPU::CPU_16BIT_INC(uint16_t &reg, uint8_t cycles)
{
    reg++;
    clockCycles += cycles;
}

void CPU::CPU_16BIT_DEC(uint16_t &reg, uint8_t cycles)
{
    AF.lo = 0;
    // 0 flag
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles);
    // subtract flag
    CPU_SET(AF.lo, FLAG_N, cycles);
    // half carry flag
    uint8_t test = reg & 0xF;
    test -= (1 & 0xF);
    if (test < 0)
        CPU_SET(AF.lo, FLAG_H, cycles);
    reg--;
    clockCycles += cycles;
}

void CPU::CPU_SBC(uint8_t reg, uint8_t cycles)
{
}

// 8-bit logic
void CPU::CPU_8BIT_AND(uint8_t &reg, uint8_t operand, uint8_t cycles, bool useImmediate)
{
    // reg = (reg & operand);
    reg &= operand;
    AF.lo = 0;
    // 0-flag
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles);
    // half carry
    CPU_SET(AF.lo, FLAG_H, cycles);
    clockCycles += cycles;
}

void CPU::CPU_8BIT_OR(uint8_t &reg, uint8_t operand, uint8_t cycles, bool useImmediate)
{
    // reg = (reg | operand);
    reg |= operand;
    AF.lo = 0;
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles);
    clockCycles += cycles;
}

void CPU::CPU_8BIT_XOR(uint8_t &reg, uint8_t operand, uint8_t cycles, bool useImmediate)
{
    reg ^= operand;
    AF.lo = 0;
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z, cycles);
    clockCycles += cycles;
}

// stack operations
void CPU::CPU_PUSH(Register reg, uint8_t cycles)
{
    // TODO: check correctness
    memory->push(reg.value, sp);
    clockCycles += cycles;
}

void CPU::CPU_POP(Register &reg, uint8_t cycles)
{
    // TODO: check for semantic errors
    reg.value = memory->pop(sp);
    clockCycles += cycles;
}

// rotates & shifts
void CPU::CPU_RLA()
{
}

void CPU::CPU_RRCA()
{
}

void CPU::CPU_RRA()
{
}

void CPU::CPU_RLCA()
{
}

void CPU::CPU_RLC()
{
}

void CPU::CPU_RL()
{
}

void CPU::CPU_RRC()
{
}

void CPU::CPU_RR()
{
}

void CPU::CPU_SLA()
{
}

void CPU::CPU_SRA()
{
}

void CPU::CPU_SRL()
{
}

// bit operations
// TODO: test bit
void CPU::CPU_BIT(uint8_t &n, uint8_t r, int cycles)
{
    int mask = 1 << r; // TODO: fix
    n &= mask;
}
// TODO: set bit
void CPU::CPU_SET(uint8_t &n, uint8_t r, int cycles)
{
    int mask = 1 << r;
    n |= mask;
}
// TODO: reset bit
void CPU::CPU_RES(uint8_t &n, uint8_t r, int cycles)
{
    n &= ~((uint8_t)1 << r);
}

// jumps
void CPU::CPU_JP(uint16_t addr)
{
    pc = addr;
}

void CPU::CPU_JR(int8_t n)
{
    pc += n;
    pc++;
}

// calls
void CPU::CPU_CALL(uint16_t nn)
{
    pc += 2;
    memory->push(pc, sp);
    pc = nn;
    return;
}
// restarts & returns
void CPU::CPU_RST(uint8_t n, int cycles)
{
    memory->push(pc, sp);
    clockCycles += cycles; // 32
    pc = n;
}

void CPU::CPU_RET()
{
    pc = memory->pop(sp);
}

void CPU::CPU_RETI()
{
    interruptsEnabled = true;
    // return
}

// Extended opcodes (CB XX)
