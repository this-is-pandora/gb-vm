#include "../include/cpu.h"

// cpu control instructions
void CPU::CPU_NOP()
{
    pc++;
}

int CPU::CPU_STOP()
{
    cpuStopped = true;
    return 0;
}

int CPU::CPU_CPL()
{
    AF.hi = (~AF.hi) & 0xFF;
    uint8_t flag = 0x60;
    AF.lo &= 0x90;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF;
    return 0;
    // return 4
}

int CPU::CPU_CCF()
{
    uint8_t flag = (AF.lo & 0x10) ^ 0x10;
    AF.lo &= 0x80;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF;
    return 0;
    // return 4
}

int CPU::CPU_SCF()
{
    uint8_t flag = 0x10;
    AF.lo &= 0x80;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF;
    return 0;
    // return 4
}

// TODO: finish
int CPU::CPU_DAA()
{
    uint8_t A = AF.hi;
    AF.lo = 0;
    // 0 flag
    if (A == 0)
        CPU_SET(AF.lo, FLAG_Z);
    // carry flag
    if (AF.hi > 0xFF)
        CPU_SET(AF.lo, FLAG_C);
    return 0;
}
int CPU::CPU_HALT()
{
    cpuHalted = true;
    return 0;
    // clo += 4;
}
int CPU::CPU_DI()
{
    interruptsEnabled = false;
    return 0;
    // clo += 4;
}
int CPU::CPU_EI()
{
    interruptsEnabled = true;
    return 0;
    // clo += 4
}

// CPU LOAD instructions
// TODO: fix register to register
void CPU::CPU_REG_LD(uint8_t &reg, uint8_t load)
{
    reg = load;
}

// memory to register
void CPU::CPU_REG_ROM_LD(uint8_t &reg, uint16_t addr)
{
    reg = memory->readByte(addr);
}

// register to memory
void CPU::CPU_ROM_REG_LD(uint16_t addr, uint8_t reg)
{
    memory->writeByte(addr, reg);
}
// immediate to memory
void CPU::CPU_8BIT_LD(uint8_t &reg)
{
    uint8_t value = memory->readByte(pc);
    pc++;
    reg = value;
}

void CPU::CPU_16BIT_LD(uint16_t &reg)
{
}

void CPU::CPU_16BIT_REG_LD(uint16_t &reg, uint16_t src)
{
}

// 8-bit arithmetic
void CPU::CPU_8BIT_ADD(uint8_t &reg, uint8_t add, bool useImmediate, bool addCarry)
{
    // TODO
    if (getCarryFlag())
        add += 1;

    AF.lo = 0;
    // zero flag
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z);
    // subtract flag = 0
    CPU_RES(AF.lo, FLAG_N);

    // half carry
    uint16_t test = (reg & 0xF); // get the lower half nibble
    test += (add & 0xF);         // lower half nibble of add
    if (test > 0xF)              // if overflow, set the half carry flag
        CPU_SET(AF.lo, FLAG_H);

    // carry flag
    if ((reg + add) > 0xFF)
        CPU_SET(AF.lo, FLAG_C);

    reg += add;
}

// TODO: same as above
void CPU::CPU_8BIT_SUB(uint8_t &reg, uint8_t sub, bool useImmediate, bool subCarry)
{
    // setFlags(true, true, true, true);

    AF.lo = 0;
    // 0 flag
    if ((reg - sub) == 0)
        CPU_SET(AF.lo, FLAG_Z);
    // subtract flag
    CPU_SET(AF.lo, FLAG_N);
    // carry flag
    if (reg < sub)
        CPU_SET(AF.lo, FLAG_C);
    // half carry flag
    uint16_t test = (reg & 0xF);
    test -= (sub & 0xF);
    if (test < 0)
        CPU_SET(AF.lo, FLAG_H);

    reg -= sub;
}
// TODO
void CPU::CPU_8BIT_INC(uint8_t &reg)
{
    AF.lo = 0;
    // zero flag
    if ((reg + 1) == 0)
        CPU_SET(AF.lo, FLAG_Z); // AF.lo, FLAG_Z
    // subtract flag
    CPU_RES(AF.lo, FLAG_N);

    // half carry flag
    if ((reg & 0xF) >= 0xF)
        CPU_SET(AF.lo, FLAG_H);

    reg++;
}

void CPU::CPU_8BIT_DEC(uint8_t &reg)
{
}

void CPU::CPU_8BIT_ADC(uint8_t reg)
{
    CPU::CPU_8BIT_ADD(AF.hi, reg, false, true);
}

void CPU::CPU_SBC(uint8_t reg)
{
    CPU_8BIT_SUB(AF.hi, reg, false, true);
}

void CPU::CPU_8BIT_CP(uint8_t &reg, uint8_t cp)
{
    uint8_t val = reg - cp;
    AF.lo = 0;
    // 0 flag
    if (val == 0)
        CPU_SET(AF.lo, FLAG_Z);
    // subtract flag
    CPU_SET(AF.lo, FLAG_N);
    // carry flag
    if (reg < cp)
        CPU_SET(AF.lo, FLAG_C);
    // TODO: half carry
    uint8_t test = reg & 0xF;
    test -= (cp & 0xF);
    if (test < 0)
        CPU_SET(AF.lo, FLAG_H);
}
// 16-bit arithmetic
void CPU::CPU_16BIT_ADD(uint16_t &reg, uint16_t add)
{
    reg += add;
}

void CPU::CPU_16BIT_SUB(uint16_t &reg, uint16_t sub)
{
    reg -= sub;
}

void CPU::CPU_16BIT_INC(uint16_t &reg)
{
    reg++;
}

void CPU::CPU_16BIT_DEC(uint16_t &reg)
{
    AF.lo = 0;
    // 0 flag
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z);
    // subtract flag
    CPU_SET(AF.lo, FLAG_N);
    // half carry flag
    uint8_t test = reg & 0xF;
    test -= (1 & 0xF);
    if (test < 0)
        CPU_SET(AF.lo, FLAG_H);
    reg--;
}

// 8-bit logic
void CPU::CPU_8BIT_AND(uint8_t &reg, uint8_t operand, bool useImmediate)
{
    // reg = (reg & operand);
    reg &= operand;
    AF.lo = 0;
    // 0-flag
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z);
    // half carry
    CPU_SET(AF.lo, FLAG_H);
}

void CPU::CPU_8BIT_OR(uint8_t &reg, uint8_t operand, bool useImmediate)
{
    // reg = (reg | operand);
    reg |= operand;
    AF.lo = 0;
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z);
}

void CPU::CPU_8BIT_XOR(uint8_t &reg, uint8_t operand, bool useImmediate)
{
    reg ^= operand;
    AF.lo = 0;
    if (reg == 0)
        CPU_SET(AF.lo, FLAG_Z);
}

// stack operations
void CPU::CPU_PUSH(Register reg)
{
    // TODO: check correctness
    memory->push(reg.value, sp);
}

void CPU::CPU_POP(Register &reg)
{
    // TODO: check for semantic errors
    reg.value = memory->pop(sp);
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
void CPU::CPU_BIT(uint8_t &n, uint8_t r)
{
    int mask = 1 << r; // TODO: fix
    n &= mask;
}
// TODO: set bit
void CPU::CPU_SET(uint8_t &n, uint8_t r)
{
    int mask = 1 << r;
    n |= mask;
}
// TODO: reset bit
void CPU::CPU_RES(uint8_t &n, uint8_t r)
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
void CPU::CPU_RST(uint8_t n)
{
    memory->push(pc, sp); // 32
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
