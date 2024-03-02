#include "../include/cpu.h"

// cpu control instructions
void CPU::CPU_NOP()
{
    pc++;
}

void CPU::CPU_STOP()
{
    cpuStopped = true;
}

void CPU::CPU_CPL()
{
    /*
    AF.hi = (~AF.hi) & 0xFF;
    uint8_t flag = 0x60;
    AF.lo &= 0x90;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF; */
    AF.hi = ~(AF.hi);
    setSubtractFlag(true);
    setHalfCarryFlag(true);
    // return 4
}

void CPU::CPU_CCF()
{
    /*
    uint8_t flag = (AF.lo & 0x10) ^ 0x10;
    AF.lo &= 0x80;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF;*/
    setSubtractFlag(false);
    setHalfCarryFlag(false);
    setCarryFlag((~(getCarryFlag()) & 1));
}

void CPU::CPU_SCF()
{
    /*
    uint8_t flag = 0x10;
    AF.lo &= 0x80;
    AF.lo |= flag;
    pc += 1;
    pc &= 0xFFFF; */
    setCarryFlag(true);
    setHalfCarryFlag(false);
    setSubtractFlag(false);
    // pc++;
}

void CPU::CPU_DAA()
{
    // uint8_t A = AF.hi;
    /*
    AF.lo = 0;
    // 0 flag
    if (A == 0)
        setZeroFlag(true);
    // carry flag
    if (AF.hi > 0xFF)
        setCarryFlag(true);*/
    if (!getSubtractFlag())
    {
        if (getCarryFlag() || AF.hi > 0x99)
        {
            AF.hi += 0x60;
            setCarryFlag(true);
        }
        if (getHalfCarryFlag() || (AF.hi & 0x0F) > 0x99)
        {
            AF.hi += 0x06;
        }
    }
    else
    {
        if (getCarryFlag())
            AF.hi -= 0x60;
        if (getHalfCarryFlag())
            AF.hi -= 0x06;
    }
    if (AF.hi == 0)
        setZeroFlag(true);
    setHalfCarryFlag(false);
    // pc+=1
}
void CPU::CPU_HALT()
{
    cpuHalted = true;
    // pc+=1;
}
void CPU::CPU_DI()
{
    // interruptsEnabled = false;
    pendingDisable = true;
}
void CPU::CPU_EI()
{
    // interruptsEnabled = true;
    pendingEnable = true;
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
    uint16_t val = memory->readWord(pc);
    pc++;
    reg = val;
}

void CPU::CPU_16BIT_REG_LD(uint16_t &reg, uint16_t src)
{
    reg = src;
}

// 8-bit arithmetic
void CPU::CPU_8BIT_ADD(uint8_t &reg, uint8_t add, bool useImmediate, bool addCarry)
{
    // TODO: finish
    if (addCarry)
        if (getCarryFlag())
            add += 1;
    AF.lo = 0;
    // zero flag
    if (reg == 0)
        setZeroFlag(true);
    // subtract flag = 0
    setSubtractFlag(false);
    // half carry
    uint16_t test = (reg & 0xF); // get the lower half nibble
    test += (add & 0xF);         // lower half nibble of add
    if (test > 0xF)              // if overflow, set the half carry flag;
        setHalfCarryFlag(true);
    // carry flag
    if ((reg + add) > 0xFF)
        setCarryFlag(true);
    reg += add;
}

// TODO: same as above
void CPU::CPU_8BIT_SUB(uint8_t &reg, uint8_t sub, bool useImmediate, bool subCarry)
{
    AF.lo = 0;
    // 0 flag
    if ((reg - sub) == 0)
        setZeroFlag(true);
    // subtract flag
    setSubtractFlag(true);
    // carry flag
    if (reg < sub)
        setCarryFlag(true);
    // half carry flag
    int16_t test = (reg & 0xF);
    test -= (sub & 0xF);
    if (test < 0)
        setHalfCarryFlag(true);
    reg -= sub;
}
// TODO
void CPU::CPU_8BIT_INC(uint8_t &reg)
{
    AF.lo = 0;
    // zero flag
    if ((reg + 1) == 0)
        setZeroFlag(true);
    // subtract flag
    setSubtractFlag(false);
    // half carry flag
    if ((reg & 0xF) >= 0xF)
        setHalfCarryFlag(true);
    reg++;
}

void CPU::CPU_8BIT_DEC(uint8_t &reg)
{
    uint8_t val = reg - 1;
    reg--;
}

void CPU::CPU_8BIT_ADC(uint8_t reg)
{
    CPU_8BIT_ADD(AF.hi, reg, false, true);
}

void CPU::CPU_SBC(uint8_t reg)
{
    CPU_8BIT_SUB(AF.hi, reg, false, true);
}

// TODO: remove reference
void CPU::CPU_8BIT_CP(uint8_t &reg, uint8_t cp)
{
    uint8_t val = reg - cp;
    AF.lo = 0;
    // 0 flag
    if (val == 0)
        setZeroFlag(true);
    // subtract flag
    setSubtractFlag(true);
    // carry flag
    if (reg < cp)
        setCarryFlag(true);
    // TODO: half carry
    int16_t test = reg & 0xF;
    test -= (cp & 0xF);
    if (test < 0)
        setHalfCarryFlag(true);
}
// 16-bit arithmetic
void CPU::CPU_16BIT_ADD(uint16_t &reg, uint16_t add)
{
    uint16_t before = reg;
    reg += add;
    setSubtractFlag(false);
    if ((before + add) > 0xFFFF)
        setCarryFlag(true);
    else
        setCarryFlag(false);
    if (((before & 0xFF00) & 0xF) + ((add >> 8) & 0xF))
        setHalfCarryFlag(true);
    else
        setHalfCarryFlag(false);
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
        setZeroFlag(true);
    // subtract flag
    setSubtractFlag(true);
    // half-carry flag
    uint8_t test = reg & 0xF;
    test -= (1 & 0xF);
    if (test < 0)
        setHalfCarryFlag(true);
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
        setZeroFlag(true);
    // half carry
    setHalfCarryFlag(true);
}

void CPU::CPU_8BIT_OR(uint8_t &reg, uint8_t operand, bool useImmediate)
{
    // reg = (reg | operand);
    reg |= operand;
    AF.lo = 0;
    if (reg == 0)
        setZeroFlag(true);
}

void CPU::CPU_8BIT_XOR(uint8_t &reg, uint8_t operand, bool useImmediate)
{
    reg ^= operand;
    AF.lo = 0;
    if (reg == 0)
        setZeroFlag(true);
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
void CPU::CPU_RLC(uint8_t &reg)
{
    reg <<= 1;
    // TODO: check lsb & set carry flag
}

void CPU::CPU_RL(uint8_t &reg)
{
    reg <<= 1;
}

void CPU::CPU_RRC(uint8_t &reg)
{
    reg >>= 1;
    // TODO: check lsb & set carry flag
}

void CPU::CPU_RR(uint8_t &reg)
{
    reg >>= 1;
}
void CPU::CPU_SLA(uint8_t &reg)
{
    reg <<= 1;
}

void CPU::CPU_SRA(uint8_t &reg)
{
    reg >>= 1;
}

void CPU::CPU_SRL(uint8_t &reg)
{
    reg >>= 1;
}
// shifts & rotate into memory
void CPU::CPU_RR_MEM(uint16_t addr)
{
    uint8_t n = memory->readByte(addr);
    n >>= 1;
    memory->writeByte(addr, n);
}

void CPU::CPU_RL_MEM(uint16_t addr)
{
    uint8_t n = memory->readByte(addr);
    n <<= 1;
    memory->writeByte(addr, n);
}

void CPU::CPU_SRA_MEM(uint16_t addr)
{
    uint8_t n = memory->readByte(addr);
    n >>= 1;
    memory->writeByte(addr, n);
}

void CPU::CPU_SLA_MEM(uint16_t addr)
{
    uint8_t n = memory->readByte(addr);
    n <<= 1;
    memory->writeByte(addr, n);
    // flag stuff
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
    // interruptsEnabled = true;
    // memory->enableInterrupts(true);
    pendingEnable = true;
    CPU_RET();
    // return
}

// Extended opcodes (CB XX)
