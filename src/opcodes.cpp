#include "cpu.h"

// cpu control instructions
void CPU::CPU_NOP()
{
    // pc++;
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
void CPU::CPU_REG_LD(uint8_t &reg, uint8_t load)
{
    reg = load;
}

// memory to register
void CPU::CPU_REG_ROM_LD(uint8_t &reg, uint16_t addr)
{
    reg = mmu->readByte(addr);
}

// register to memory
void CPU::CPU_ROM_REG_LD(uint16_t addr, uint8_t reg)
{
    mmu->writeByte(addr, reg);
}
// immediate to memory
void CPU::CPU_8BIT_LD(uint8_t &reg)
{
    uint8_t value = mmu->readByte(pc);
    pc++;
    reg = value;
}

void CPU::CPU_16BIT_LD(uint16_t &reg)
{
    uint16_t val = mmu->readWord(pc); // TODO: check
    pc += 2;
    reg = val;
}

void CPU::CPU_16BIT_REG_LD(uint16_t &reg, uint16_t src)
{
    reg = src;
}

// 8-bit arithmetic
void CPU::CPU_8BIT_ADD(uint8_t &reg, uint8_t add, bool useImmediate, bool addCarry)
{
    if (useImmediate)
    {
        add = readByte(pc++);
    }
    if (addCarry)
        if (getCarryFlag())
            add += 1;
    AF.lo = 0;
    // subtract flag = 0
    // half carry
    uint16_t test = (reg & 0xF); // get the lower half nibble of reg
    test += (add & 0xF);         // lower half nibble of add
    if (test > 0xF)              // if overflow, set the half carry flag;
        setHalfCarryFlag(true);
    // carry flag
    if ((reg + add) > 0xFF)
        setCarryFlag(true);
    reg += add;
    // zero flag
    if (reg == 0)
        setZeroFlag(true);
}

void CPU::CPU_8BIT_SUB(uint8_t &reg, uint8_t sub, bool useImmediate, bool subCarry)
{
    if (useImmediate)
        sub = readByte(pc++);
    if (subCarry)
    {
        if (getCarryFlag())
            sub++;
    }
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

void CPU::CPU_8BIT_INC(uint8_t &reg)
{
    // AF.lo = 0;
    //  zero flag
    if ((reg + 1) == 0)
        setZeroFlag(true);
    else
        setZeroFlag(false);
    // subtract flag
    setSubtractFlag(false);
    // half carry flag
    if ((reg & 0xF) == 0xF)
        setHalfCarryFlag(true);
    else
        setHalfCarryFlag(false);
    reg += 1;
}

void CPU::CPU_8BIT_DEC(uint8_t &reg)
{
    setSubtractFlag(true);
    if ((reg & 0xf) == 0)
    {
        setHalfCarryFlag(true);
    }
    else
    {
        setHalfCarryFlag(false);
    }
    reg -= 1;
    // z-flag
    if (reg == 0)
        setZeroFlag(true);
    else
        setZeroFlag(false);
}

void CPU::CPU_8BIT_ADC(uint8_t reg)
{
    CPU_8BIT_ADD(AF.hi, reg, false, true);
}

void CPU::CPU_SBC(uint8_t reg)
{
    CPU_8BIT_SUB(AF.hi, reg, false, true);
}

void CPU::CPU_8BIT_CP(uint8_t reg, uint8_t cp)
{
    uint8_t val;
    /*
    if (reg < cp)
        val = 0;
    else
        val = reg - cp;*/
    val = reg - cp;
    AF.lo = 0;
    // 0 flag
    if (val == 0)
        setZeroFlag(true);
    // subtract flag
    setSubtractFlag(true);
    // carry flag
    if (reg < cp)
        setCarryFlag(true);
    // half carry
    int16_t test = reg & 0xF;
    test -= (cp & 0xF);
    if (test < 0)
        setHalfCarryFlag(true);
}
// 16-bit arithmetic
void CPU::CPU_16BIT_ADD(uint16_t &reg, uint16_t add)
{
    // AF.lo = 0;
    uint16_t before = reg;
    reg += add;
    setSubtractFlag(false);
    // carry flag
    if ((before + add) > 0xFFFF)
        setCarryFlag(true);
    else
        setCarryFlag(false);
    // half carry falg
    if (((before & 0xFF00) & 0xF) + ((add >> 8) & 0xF))
        setHalfCarryFlag(true);
    else
        setHalfCarryFlag(false);
}

void CPU::CPU_16BIT_SUB(uint16_t &reg, uint16_t sub)
{
    AF.lo = 0;
    setSubtractFlag(true);
    if (reg == 0)
        setZeroFlag(true);
    // TODO: Half carry flag & carry flag
    if (reg < sub)
        setCarryFlag(true);
    uint16_t test = reg & 0xF;
    test -= (sub & 0xF);
    if (test < 0)
        setHalfCarryFlag(true);
    reg -= sub;
}

void CPU::CPU_16BIT_INC(uint16_t &reg)
{
    reg++;
}

void CPU::CPU_16BIT_DEC(uint16_t &reg)
{
    reg--;
}

// 8-bit logic
void CPU::CPU_8BIT_AND(uint8_t &reg, uint8_t operand, bool useImmediate)
{
    // reg = (reg & operand);
    if (useImmediate)
        operand = mmu->readByte(pc++);
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
    if (useImmediate)
        operand = mmu->readByte(pc++);
    reg |= operand;
    AF.lo = 0;
    if (reg == 0)
        setZeroFlag(true);
}

void CPU::CPU_8BIT_XOR(uint8_t &reg, uint8_t operand, bool useImmediate)
{
    if (useImmediate)
        operand = mmu->readByte(pc++);
    reg ^= operand;
    AF.lo = 0;
    if (reg == 0)
        setZeroFlag(true);
}

// stack operations
void CPU::CPU_PUSH(Register reg)
{
    mmu->push(reg.value, sp);
}

void CPU::CPU_POP(Register &reg)
{
    reg.value = mmu->pop(sp);
}

// rotates & shifts
void CPU::CPU_RLC(uint8_t &reg)
{
    if ((reg >> 7) & 0x01)
        setCarryFlag(true);
    setSubtractFlag(false);
    setHalfCarryFlag(false);
    reg = (reg << 1) | getCarryFlag();
    if (reg == 0)
        setZeroFlag(true);
}

void CPU::CPU_RL(uint8_t &reg)
{
    int oldcarry = getCarryFlag();
    AF.lo = 0;
    if ((reg >> 7) & 0x01)
        setCarryFlag(true);
    reg = (reg << 1) | oldcarry;
    if (reg == 0)
        setZeroFlag(true);
}
// rotate right to carry
void CPU::CPU_RRC(uint8_t &reg)
{
    if (reg & 0x01)
        setCarryFlag(true);
    setSubtractFlag(false);
    setHalfCarryFlag(false);
    reg = (reg >> 1) | (getCarryFlag() << 7);
    if (reg == 0)
        setZeroFlag(true);
}
// rotate right through carry
void CPU::CPU_RR(uint8_t &reg)
{
    int oldcarry = getCarryFlag();
    AF.lo = 0;
    if (reg & 0x01)
        setCarryFlag(true);
    reg = (reg >> 1) | (7 << oldcarry);
    if (reg == 0)
        setZeroFlag(true);
}
void CPU::CPU_SLA(uint8_t &reg)
{
    AF.lo = 0;
    if ((reg >> 7) & 0x01)
        setCarryFlag(true);
    reg <<= 1;
    if (reg == 0)
        setZeroFlag(true);
}

void CPU::CPU_SRA(uint8_t &reg)
{
    int msb = (reg >> 7) & 0x01;
    AF.lo = 0;
    if (reg & 0x01)
        setCarryFlag(true);
    reg = (reg >> 1) | (msb << 7);
    if (reg == 0)
        setZeroFlag(true);
}

void CPU::CPU_SRL(uint8_t &reg)
{
    AF.lo = 0;
    if (reg & 0x01)
        setCarryFlag(true);
    reg >>= 1;
    if (reg == 0)
        setZeroFlag(true);
    // pc += 2;
}
// shifts & rotate into memory
void CPU::CPU_RR_MEM(uint16_t addr)
{
    uint8_t n = mmu->readByte(addr);
    int oldcarry = getCarryFlag();
    AF.lo = 0;
    if (n & 0x01)
        setCarryFlag(true);
    n = (n >> 1) | (7 << oldcarry);
    if (n == 0)
        setZeroFlag(true);
    mmu->writeByte(addr, n);
}

void CPU::CPU_RL_MEM(uint16_t addr)
{
    uint8_t n = mmu->readByte(addr);
    int oldcarry = getCarryFlag();
    AF.lo = 0;
    if ((n >> 7) & 0x01)
        setCarryFlag(true);
    n = (n << 1) | oldcarry;
    if (n == 0)
        setZeroFlag(true);
    mmu->writeByte(addr, n);
}

void CPU::CPU_SRA_MEM(uint16_t addr)
{
    uint8_t n = mmu->readByte(addr);
    int msb = (n >> 7) & 0x01;
    AF.lo = 0;
    if (n & 0x01)
        setCarryFlag(true);
    n = (n >> 1) | (msb << 7);
    if (n == 0)
        setZeroFlag(true);
    mmu->writeByte(addr, n);
}

void CPU::CPU_SLA_MEM(uint16_t addr)
{
    uint8_t n = mmu->readByte(addr);
    /*
    AF.lo = 0;
    if ((n >> 7) & 0x01)
        setCarryFlag(true);
    n <<= 1;
    if (n == 0)
        setZeroFlag(true);*/
    CPU_SLA(n);
    mmu->writeByte(addr, n);
}

void CPU::CPU_SRL_MEM(uint16_t addr)
{
    uint8_t n = readByte(addr);
    /*
    AF.lo = 0;
    if (n & 0x01)
        setCarryFlag(true);
    n >>= 1;
    if (n == 0)
        setZeroFlag(true);*/
    CPU_SRL(n);
    writeByte(addr, n);
}

void CPU::CPU_RLC_MEM(uint16_t addr)
{
    uint8_t n = readByte(addr);
    CPU_RLC(n);
    writeByte(addr, n);
}

void CPU::CPU_RRC_MEM(uint16_t addr)
{
    uint8_t n = readByte(addr);
    CPU_RRC(n);
    writeByte(addr, n);
}

// bit operations
void CPU::CPU_BIT(uint8_t n, uint8_t r)
{
    // AF.lo = 0;
    if (~(n >> r) & 0x1)
        setZeroFlag(true);
    else
        setZeroFlag(false);
    setSubtractFlag(false);
    setHalfCarryFlag(true);
}

void CPU::CPU_SET(uint8_t &n, uint8_t r)
{
    int mask = 1 << r;
    n |= mask;
}

void CPU::CPU_RES(uint8_t &n, uint8_t r)
{
    n &= ~(1 << r);
}

void CPU::CPU_SWAP(uint8_t &n)
{
    n = ((n >> 4) & 0xF) | ((n << 4) & 0xF0);
    AF.lo = 0;
    if (n == 0)
        setZeroFlag(true);
    // pc += 2
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
    mmu->push(pc, sp);
    pc = nn;
}
// restarts & returns
void CPU::CPU_RST(uint8_t n)
{
    mmu->push(pc, sp); // 32
    pc = n;
}

void CPU::CPU_RET()
{
    pc = mmu->pop(sp);
}

void CPU::CPU_RETI()
{
    // interruptsEnabled = true;
    // memory->enableInterrupts(true);
    pendingEnable = true;
    CPU_RET();
    // return
}
