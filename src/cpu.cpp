#include "cpu.h"
#include <iostream>
#include <thread>

CPU::CPU(std::shared_ptr<MMU> mmu) : mmu(mmu)
{
    // boot rom
    AF.value = 0x01B0;
    BC.value = 0x0013;
    DE.value = 0x00D8;
    HL.value = 0x014D;

    clocksum = 0;
    clockSpeed = 1024;

    accessedMemory = false;
    cpuStopped = false;
    cpuHalted = false;
    debugMode = false;
    pc = 0x0;
    // pc = 0x100;
    sp = 0x0;
    pendingDisable = false;
    pendingEnable = false;
}

/* output current CPU register values*/
void CPU::status()
{
    printf("a = %02x\n", AF.hi);
    printf("f = %02x\n", AF.lo);
    printf("b = %02x\n", BC.hi);
    printf("c = %02x\n", BC.lo);
    printf("d = %02x\n", DE.hi);
    printf("e = %02x\n", DE.lo);
    printf("h = %02x\n", HL.hi);
    printf("l = %02x\n", HL.lo);
    printf("sp = %04x\n", sp);
    printf("pc = %04x\n", pc);
    printf("c_flag = %u\n", getCarryFlag());
    printf("h_flag = %u\n", getHalfCarryFlag());
    printf("n_flag = %u\n", getSubtractFlag());
    printf("z_flag = %u\n\n", getZeroFlag());
}

uint16_t CPU::getPC()
{
    return pc;
}

void CPU::setPC(uint16_t _pc)
{
    pc = _pc;
}

uint16_t CPU::getSP()
{
    return sp;
}

void CPU::setSP(uint16_t _sp)
{
    sp = _sp;
}

uint16_t CPU::getOP()
{
    return mmu->readByte(pc);
}

void CPU::writeByte(uint16_t addr, uint8_t data)
{
    // mmu->writeByte(addr, data);
    mmu->writeByte(addr, data);
}

void CPU::writeWord(uint16_t addr, uint16_t data)
{
    // mmu->writeWord(addr, data);
    mmu->writeWord(addr, data);
}

uint8_t CPU::readByte(uint16_t addr)
{
    // uint8_t data = mmu->readByte(addr);
    uint8_t data = mmu->readByte(addr);
    return data;
}

uint16_t CPU::readWord(uint16_t addr)
{
    // uint16_t data = mmu->readWord(addr);
    uint16_t data = mmu->readWord(addr);
    return data;
}

bool CPU::getZeroFlag() const
{
    return (AF.lo & FLAG_MASK_Z) ? true : false;
}
void CPU::setZeroFlag(bool val)
{
    if (val)
    {
        CPU_SET(AF.lo, FLAG_Z);
    }
    else
    {
        CPU_RES(AF.lo, FLAG_Z);
    }
}

bool CPU::getSubtractFlag() const
{
    return (AF.lo & FLAG_MASK_N) ? true : false;
}
void CPU::setSubtractFlag(bool val)
{
    if (val)
    {
        CPU_SET(AF.lo, FLAG_N);
    }
    else
    {
        CPU_RES(AF.lo, FLAG_N);
    }
}

bool CPU::getCarryFlag() const
{
    return (AF.lo & FLAG_MASK_C) ? true : false;
}

void CPU::setCarryFlag(bool val)
{
    if (val)
    {
        CPU_SET(AF.lo, FLAG_C);
    }
    else
    {
        CPU_RES(AF.lo, FLAG_C);
    }
}

bool CPU::getHalfCarryFlag() const
{
    return (AF.lo & FLAG_MASK_H) ? true : false;
}

void CPU::setHalfCarryFlag(bool val)
{
    if (val)
    {
        CPU_SET(AF.lo, FLAG_H);
    }
    else
    {
        CPU_RES(AF.lo, FLAG_H);
    }
}

uint8_t CPU::fetch()
{
    uint8_t data = mmu->readByte(pc++);
    return data;
    // return m_ROM[pc++];
}
// TODO: instructions to test: 0x20, 0x90, 0xF0, 0xFE
int CPU::execute(uint8_t opcode)
{
    bool ext_op = false; // place holder code for now
    switch (opcode)
    {
    case 0x00: // NOP
        CPU_NOP();
        break;
    case 0x01: // ld bc, imm
        CPU_16BIT_LD(BC.value);
        break;
    case 0x02: // ld (bc), a
        CPU_ROM_REG_LD(BC.value, AF.hi);
        break;
    case 0x03:
        CPU_16BIT_INC(BC.value);
        break;
    case 0x04:
        CPU_8BIT_INC(BC.hi);
        break;
    case 0x05:
        CPU_8BIT_DEC(BC.hi);
        break;
    case 0x06:
        CPU_8BIT_LD(BC.hi);
        break;
    case 0x07: // TODO: RLCA
        CPU_RLC(AF.hi);
        break;
    case 0x08: // TODO: LD nn, sp
    {
        uint16_t nn = readWord(pc);
        pc += 2;
        writeByte(nn++, sp);
        writeByte(nn, sp); // TODO: change this to writeWord?
    }
    break;
    case 0x09:
        CPU_16BIT_ADD(HL.value, BC.value);
        break;
    case 0x0A:
        CPU_REG_ROM_LD(AF.hi, BC.value);
        break;
    case 0x0B:
        CPU_16BIT_DEC(BC.value);
        break;
    case 0x0C:
        CPU_8BIT_INC(BC.lo);
        break;
    case 0x0D:
        CPU_8BIT_DEC(BC.lo);
        break;
    case 0x0E:
        CPU_8BIT_LD(BC.lo);
        break;
    case 0x0F:
        CPU_RRC(AF.hi);
        break;
    case 0x10: // TODO stop
        CPU_STOP();
        break;
    case 0x11: // LD DE, nn
        CPU_16BIT_LD(DE.value);
        break;
    case 0x12: // LD (DE), A
        CPU_ROM_REG_LD(DE.value, AF.hi);
        break;
    case 0x13:
        CPU_16BIT_INC(DE.value);
        break;
    case 0x14:
        CPU_8BIT_INC(DE.hi);
        break;
    case 0x15:
        CPU_8BIT_DEC(DE.hi);
        break;
    case 0x16:
        CPU_8BIT_LD(DE.hi);
        break;
    case 0x17:
        CPU_RL(AF.hi);
        break;
    case 0x18: // TODO: JR n
    {
        int8_t n = (int8_t)readByte(pc);
        CPU_JR(n);
    }
    break;
    case 0x19:
        CPU_16BIT_ADD(HL.value, DE.value);
        break;
    case 0x1A:
        CPU_REG_ROM_LD(AF.hi, DE.value);
        break;
    case 0x1B:
        CPU_16BIT_DEC(DE.value);
        break;
    case 0x1C:
        CPU_8BIT_INC(DE.lo);
        break;
    case 0x1D:
        CPU_8BIT_DEC(DE.lo);
        break;
    case 0x1E:
        CPU_8BIT_LD(DE.lo);
        break;
    case 0x1F:
        CPU_RR(AF.hi);
        break;
    case 0x20: // TODO: check JR NZ, *
    {
        int8_t n = (int8_t)readByte(pc); // TODO check
        // printf("B Reg: %.2X\n", BC.hi);
        // printf("Z-Flag: %d\n", getZeroFlag());
        if (getZeroFlag() == false)
            CPU_JR(n);
        else
            pc++;
    }
    break;
    case 0x21:
        CPU_16BIT_LD(HL.value);
        break;
    case 0x22:
    {
        CPU_ROM_REG_LD(HL.value, AF.hi);
        CPU_16BIT_INC(HL.value);
    }
    break;
    case 0x23:
        CPU_16BIT_INC(HL.value);
        break;
    case 0x24:
        CPU_8BIT_INC(HL.hi);
        break;
    case 0x25:
        CPU_8BIT_DEC(HL.hi);
        break;
    case 0x26:
        CPU_8BIT_LD(HL.hi);
        break;
    case 0x27: // decimal adjust register A (DAA)
        CPU_DAA();
        break;
    case 0x28: // TODO: JR Z,*
    {
        int8_t n = (int8_t)readByte(pc);
        if (getZeroFlag() == true)
            CPU_JR(n);
        else
            pc++;
    }
    break;
    case 0x29:
        CPU_16BIT_ADD(HL.value, HL.value);
        break;
    case 0x2A:
    {
        CPU_REG_ROM_LD(AF.hi, HL.value);
        CPU_16BIT_INC(HL.value);
    }
    break;
    case 0x2B:
        CPU_16BIT_DEC(HL.value);
        break;
    case 0x2C:
        CPU_8BIT_INC(HL.lo);
        break;
    case 0x2D:
        CPU_8BIT_DEC(HL.lo);
        break;
    case 0x2E:
        CPU_8BIT_LD(HL.lo);
        break;
    case 0x2F:
        CPU_CPL();
        break;
    case 0x30: // TODO: check JR NC,*
    {
        int8_t n = (int8_t)readByte(pc);
        if (getCarryFlag() == false)
            CPU_JR(n);
        else
            pc++;
    }
    break;
    case 0x31:
        CPU_16BIT_LD(sp);
        break;
    case 0x32: // LD (HL-),A
    {
        CPU_ROM_REG_LD(HL.value, AF.hi);
        CPU_16BIT_DEC(HL.value);
    }
    break;
    case 0x33:
        CPU_16BIT_INC(sp);
        break;
    case 0x34: // TODO: check
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_INC(n);
    }
    break;
    case 0x35: // TODO: FIX
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_DEC(n);
    }
    break;
    case 0x36: // TODO
    {
        uint8_t n = readByte(pc);
        pc++;
        writeByte(HL.value, n);
        // clockCycles +;
    }
    break;
    case 0x37: // TODO
        CPU_SCF();
        break;
    case 0x38: // TODO: JR C,*
    {
        int8_t n = (int8_t)readByte(pc);
        if (getCarryFlag() == true)
            CPU_JR(n);
        else
            pc++;
    }
    break;
    case 0x39:
        CPU_16BIT_ADD(HL.value, sp);
        break;
    case 0x3A: // LD A,(HLD)
    {
        CPU_REG_LD(AF.hi, HL.value);
        CPU_16BIT_DEC(HL.value);
    }
    break;
    case 0x3B:
        CPU_16BIT_DEC(sp);
        break;
    case 0x3C:
        CPU_8BIT_INC(AF.hi);
        break;
    case 0x3D:
        CPU_8BIT_DEC(AF.hi);
        break;
    case 0x3E: // TODO: LD A, n
    {
        uint8_t n = readByte(pc++);
        AF.hi = n;
        // pc++;
    }
    break;
    case 0x3F:
        CPU_CCF();
        break;
    case 0x40:
        CPU_REG_LD(BC.hi, BC.hi);
        break;
    case 0x41:
        CPU_REG_LD(BC.hi, BC.lo);
        break;
    case 0x42:
        CPU_REG_LD(BC.hi, DE.hi);
        break;
    case 0x43:
        CPU_REG_LD(BC.hi, DE.lo);
        break;
    case 0x44:
        CPU_REG_LD(BC.hi, HL.hi);
        break;
    case 0x45:
        CPU_REG_LD(BC.hi, HL.lo);
        break;
    case 0x46:
        CPU_REG_ROM_LD(BC.hi, HL.value);
        break;
    // case 0x47:
    case 0x48:
        CPU_REG_LD(BC.lo, BC.hi);
        break;
    case 0x49:
        CPU_REG_LD(BC.lo, BC.lo);
        break;
    case 0x4A:
        CPU_REG_LD(BC.lo, DE.hi);
        break;
    case 0x4B:
        CPU_REG_LD(BC.lo, DE.lo);
        break;
    case 0x4C:
        CPU_REG_LD(BC.lo, HL.hi);
        break;
    case 0x4D:
        CPU_REG_LD(BC.lo, HL.lo);
        break;
    case 0x4E:
        CPU_REG_ROM_LD(BC.lo, HL.value); // ld C, (HL)
        break;
    case 0x4F: // LD C,A
        CPU_REG_LD(BC.lo, AF.hi);
        break;
    case 0x50: // LD D,B
        CPU_REG_LD(DE.hi, BC.hi);
        break;
    case 0x51:
        CPU_REG_LD(DE.hi, BC.lo);
        break;
    case 0x52:
        CPU_REG_LD(DE.hi, DE.hi);
        break;
    case 0x53:
        CPU_REG_LD(DE.hi, DE.lo);
        break;
    case 0x54:
        CPU_REG_LD(DE.hi, HL.hi);
        break;
    case 0x55:
        CPU_REG_LD(DE.hi, HL.lo);
        break;
    case 0x56:
        CPU_REG_ROM_LD(DE.hi, HL.value);
        break;
    case 0x57:
        CPU_REG_LD(DE.hi, AF.hi);
        break;
    case 0x58:
        CPU_REG_LD(DE.lo, BC.hi);
        break;
    case 0x59:
        CPU_REG_LD(DE.lo, BC.lo);
        break;
    case 0x5A:
        CPU_REG_LD(DE.lo, DE.hi);
        break;
    case 0x5B:
        CPU_REG_LD(DE.lo, DE.lo);
        break;
    case 0x5C:
        CPU_REG_LD(DE.lo, HL.hi);
        break;
    case 0x5D:
        CPU_REG_LD(DE.lo, HL.lo);
        break;
    case 0x5E:
        CPU_REG_ROM_LD(DE.lo, HL.value);
        break;
    case 0x5F: // LD E,A
        CPU_REG_LD(DE.lo, AF.hi);
        break;
    case 0x60: // LD H,B
        CPU_REG_LD(HL.hi, BC.hi);
        break;
    case 0x61: // LD H,C
        CPU_REG_LD(HL.hi, BC.lo);
        break;
    case 0x62: // LD H,D
        CPU_REG_LD(HL.hi, DE.hi);
        break;
    case 0x63: // LD H,E
        CPU_REG_LD(HL.hi, DE.lo);
        break;
    case 0x64: // LD H,H
        CPU_REG_LD(HL.hi, HL.hi);
        break;
    case 0x65:
        CPU_REG_LD(HL.hi, HL.lo);
        break;
    case 0x66:
        CPU_REG_ROM_LD(HL.hi, HL.value);
        break;
    case 0x67: // LD H,A
        CPU_REG_LD(HL.hi, AF.hi);
        break;
    case 0x68:
        CPU_REG_LD(HL.lo, BC.hi);
        break;
    case 0x69:
        CPU_REG_LD(HL.lo, BC.lo);
        break;
    case 0x6A:
        CPU_REG_LD(HL.lo, DE.hi);
        break;
    case 0x6B:
        CPU_REG_LD(HL.lo, DE.lo);
        break;
    case 0x6C:
        CPU_REG_LD(HL.lo, HL.hi);
        break;
    case 0x6D:
        CPU_REG_LD(HL.lo, HL.lo);
        break;
    case 0x6E:
        CPU_REG_ROM_LD(HL.lo, HL.value);
        break;
    case 0x6F: // LD L,A
        CPU_REG_LD(HL.lo, AF.hi);
        break;
    case 0x70: // LD (HL),B
        CPU_ROM_REG_LD(HL.value, BC.hi);
        break;
    case 0x71:
        CPU_ROM_REG_LD(HL.value, BC.lo);
        break;
    case 0x72:
        CPU_ROM_REG_LD(HL.value, DE.hi);
        break;
    case 0x73:
        CPU_ROM_REG_LD(HL.value, DE.lo);
        break;
    case 0x74:
        CPU_ROM_REG_LD(HL.value, HL.hi);
        break;
    case 0x75:
        CPU_ROM_REG_LD(HL.value, HL.lo);
        break;
    case 0x76: // TODO halt
        CPU_HALT();
        break;
    case 0x77:
        CPU_ROM_REG_LD(HL.value, AF.hi);
        break;
    case 0x78:
        CPU_REG_LD(AF.hi, BC.hi);
        break;
    case 0x79:
        CPU_REG_LD(AF.hi, BC.lo);
        break;
    case 0x7A:
        CPU_REG_LD(AF.hi, DE.hi);
        break;
    case 0x7B:
        CPU_REG_LD(AF.hi, DE.lo);
        break;
    case 0x7C:
        CPU_REG_LD(AF.hi, HL.hi);
        break;
    case 0x7D:
        CPU_REG_LD(AF.hi, HL.lo);
        break;
    case 0x7E:
        CPU_REG_ROM_LD(AF.hi, HL.value);
        break;
    case 0x7F:
        CPU_REG_LD(AF.hi, AF.hi);
        break;
    case 0x80:
        CPU_8BIT_ADD(AF.hi, BC.hi, false, false);
        break;
    case 0x81:
        CPU_8BIT_ADD(AF.hi, BC.lo, false, false);
        break;
    case 0x82:
        CPU_8BIT_ADD(AF.hi, DE.hi, false, false);
        break;
    case 0x83:
        CPU_8BIT_ADD(AF.hi, DE.lo, false, false);
        break;
    case 0x84:
        CPU_8BIT_ADD(AF.hi, HL.hi, false, false);
        break;
    case 0x85:
        CPU_8BIT_ADD(AF.hi, HL.lo, false, false);
        break;
    case 0x86: // TODO: ADD A, (HL)
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_ADD(AF.hi, n, false, false);
    }
    break;
    case 0x87:
        CPU_8BIT_ADD(AF.hi, AF.hi, false, false);
        break;
    case 0x88:
        CPU_8BIT_ADC(BC.hi);
        break;
    case 0x89:
        CPU_8BIT_ADC(BC.lo);
        break;
    case 0x8A:
        CPU_8BIT_ADC(DE.hi);
        break;
    case 0x8B:
        CPU_8BIT_ADC(DE.lo);
        break;
    case 0x8C:
        CPU_8BIT_ADC(HL.hi);
        break;
    case 0x8D:
        CPU_8BIT_ADC(HL.lo);
        break;
    case 0x8E: // TODO: ADC A, (HL)
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_ADC(n);
    }
    break;
    case 0x8F:
        CPU_8BIT_ADC(AF.hi);
        break;
    case 0x90:
        CPU_8BIT_SUB(AF.hi, BC.hi, false, false);
        break;
    case 0x91:
        CPU_8BIT_SUB(AF.hi, BC.lo, false, false);
        break;
    case 0x92:
        CPU_8BIT_SUB(AF.hi, DE.hi, false, false);
        break;
    case 0x93:
        CPU_8BIT_SUB(AF.hi, DE.lo, false, false);
        break;
    case 0x94:
        CPU_8BIT_SUB(AF.hi, HL.hi, false, false);
        break;
    case 0x95:
        CPU_8BIT_SUB(AF.hi, HL.lo, false, false);
        break;
    case 0x96:
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_SUB(AF.hi, n, false, false);
    }
    break;
    case 0x97:
        CPU_8BIT_SUB(AF.hi, AF.hi, false, false);
        break;
    case 0x98:
        CPU_SBC(BC.hi);
        break;
    case 0x99:
        CPU_SBC(BC.lo);
        break;
    case 0x9A:
        CPU_SBC(DE.hi);
        break;
    case 0x9B:
        CPU_SBC(DE.lo);
        break;
    case 0x9C:
        CPU_SBC(HL.hi);
        break;
    case 0x9D:
        CPU_SBC(HL.lo);
        break;
    case 0x9E:
    {
        uint8_t n = readByte(HL.value);
        CPU_SBC(n);
    }
    break;
    case 0x9F:
        CPU_SBC(AF.hi);
        break;
    case 0xA0:
        CPU_8BIT_AND(AF.hi, BC.hi, false);
        break;
    case 0xA1:
        CPU_8BIT_AND(AF.hi, BC.lo, false);
        break;
    case 0xA2:
        CPU_8BIT_AND(AF.hi, DE.hi, false);
        break;
    case 0xA3:
        CPU_8BIT_AND(AF.hi, DE.lo, false);
        break;
    case 0xA4:
        CPU_8BIT_AND(AF.hi, HL.hi, false);
        break;
    case 0xA5:
        CPU_8BIT_AND(AF.hi, HL.lo, false);
        break;
    case 0xA6:
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_AND(AF.hi, n, false);
    }
    break;
    case 0xA7:
        CPU_8BIT_AND(AF.hi, AF.hi, false);
        break;
    case 0xA8:
        CPU_8BIT_XOR(AF.hi, BC.hi, false);
        break;
    case 0xA9:
        CPU_8BIT_XOR(AF.hi, BC.lo, false);
        break;
    case 0xAA:
        CPU_8BIT_XOR(AF.hi, DE.hi, false);
        break;
    case 0xAB:
        CPU_8BIT_XOR(AF.hi, DE.lo, false);
        break;
    case 0xAC:
        CPU_8BIT_XOR(AF.hi, HL.hi, false);
        break;
    case 0xAD:
        CPU_8BIT_XOR(AF.hi, HL.lo, false);
        break;
    case 0xAE:
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_XOR(AF.hi, n, false);
    }
    break;
    case 0xAF:
        CPU_8BIT_XOR(AF.hi, AF.hi, false);
        break;
    case 0xB0:
        CPU_8BIT_OR(AF.hi, BC.hi, false);
        break;
    case 0xB1:
        CPU_8BIT_OR(AF.hi, BC.lo, false);
        break;
    case 0xB2:
        CPU_8BIT_OR(AF.hi, DE.hi, false);
        break;
    case 0xB3:
        CPU_8BIT_OR(AF.hi, DE.lo, false);
        break;
    case 0xB4:
        CPU_8BIT_OR(AF.hi, HL.hi, false);
        break;
    case 0xB5:
        CPU_8BIT_OR(AF.hi, HL.lo, false);
        break;
    case 0xB6:
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_OR(AF.hi, n, false);
    }
    break;
    case 0xB7:
        CPU_8BIT_OR(AF.hi, AF.hi, false);
        break;
    case 0xB8:
        CPU_8BIT_CP(AF.hi, BC.hi);
        break;
    case 0xB9:
        CPU_8BIT_CP(AF.hi, BC.lo);
        break;
    case 0xBA:
        CPU_8BIT_CP(AF.hi, DE.hi);
        break;
    case 0xBB:
        CPU_8BIT_CP(AF.hi, DE.lo);
        break;
    case 0xBC:
        CPU_8BIT_CP(AF.hi, HL.hi);
        break;
    case 0xBD:
        CPU_8BIT_CP(AF.hi, HL.lo);
        break;
    case 0xBE:
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_CP(AF.hi, n);
    }
    break;
    case 0xBF:
        CPU_8BIT_CP(AF.hi, AF.hi);
        break;
    case 0xC0: // TODO: RET NZ
    {
        if (getZeroFlag() == false)
            CPU_RET();
    }
    break;
    case 0xC1:
        CPU_POP(BC);
        break;
    case 0xC2: // TODO: JP NZ,nn
    {
        // uint16_t nn = mmu->readWord(pc);
        uint16_t nn = mmu->readWord(pc);
        if (getZeroFlag() == 0)
            CPU_JP(nn);
    }
    break;
    case 0xC3: // TODO
    {
        uint16_t nn = mmu->readWord(pc);
        CPU_JP(nn);
    }
    break;
    case 0xC4: // TODO: call NZ,nn
    {
        uint16_t nn = mmu->readWord(pc);
        if (getZeroFlag() == 0)
            CPU_CALL(nn);
    }
    break;
    case 0xC5:
        CPU_PUSH(BC);
        break;
    case 0xC6: // ADD A, n
        CPU_8BIT_ADD(AF.hi, 0, true, false);
        break;
    case 0xC7: // TODO: RST 00H
        CPU_RST(0x00);
        break;
    case 0xC8: // TODO: RET Z
    {
        if (getZeroFlag() == true)
            CPU_RET();
    }
    break;
    case 0xC9: // TODO: RET
        CPU_RET();
        break;
    case 0xCA: // TODO: JP Z,nn
    {
        uint16_t nn = mmu->readWord(pc);
        if (getZeroFlag() == 1)
            CPU_JP(nn);
    }
    break;
    case 0xCB: // prefix CB
    {
        executeExtendedOpcode();
        ext_op = true;
    }
    break;
    case 0xCC: // TODO: call Z,nn
    {
        uint16_t nn = readWord(pc);
        if (getZeroFlag() == 1)
            CPU_CALL(nn);
    }
    break;
    case 0xCD: // TODO: CALL nn
    {
        uint16_t nn = mmu->readWord(pc);
        CPU_CALL(nn);
    }
    break;
    case 0xCE: // ADC A, #
        CPU_8BIT_ADD(AF.hi, 0, true, true);
        break;
    case 0xCF: // RST 08H
        CPU_RST(0x08);
        break;
    case 0xD0: // TODO: RET NC
    {
        if (getCarryFlag() == false)
            CPU_RET();
    }
    break;
    case 0xD1:
        CPU_POP(DE);
        break;
    case 0xD2: // TODO: JP NC,nn
    {
        uint16_t nn = readWord(pc);
        if (getCarryFlag() == false)
            CPU_JP(nn);
    }
    break;
    // case 0xD3: // TODO: None?
    case 0xD4: // TODO: call NC,nn
    {
        uint16_t nn = readWord(pc);
        if (getCarryFlag() == false)
            CPU_CALL(nn);
    }
    break;
    case 0xD5:
        CPU_PUSH(DE);
        break;
    case 0xD6:
        CPU_8BIT_SUB(AF.hi, 0, true, false);
        break;
    case 0xD7: // TODO: RST 10H
        CPU_RST(0x10);
        break;
    case 0xD8: // TODO: RET C
    {
        if (getCarryFlag() == true)
            CPU_RET();
    }
    break;
    case 0xD9: // TODO: RETI (return from interrupt)
    {
        CPU_RET();
        CPU_EI();
    }
    break;
    case 0xDA: // TODO: JP C, nn
    {
        uint16_t nn = readWord(pc);
        if (getCarryFlag() == 1)
            CPU_JP(nn);
    }
    break;
    // case 0xDB: // TODO: None?
    case 0xDC: // TODO: call C,nn
    {
        uint16_t nn = readWord(pc);
        if (getCarryFlag() == 1)
        {
            CPU_CALL(nn);
        }
    }
    break;
    // case 0xDD: // TODO: None?
    case 0xDE: // TODO: SBC A,d8
        CPU_SBC(AF.hi);
        break;
    case 0xDF: // TODO: RST 18H
        CPU_RST(0x18);
        break;
    case 0xE0: // TODO: test
    {
        uint8_t n = readByte(pc++);
        uint16_t addr = 0xFF00 + n;
        writeByte(addr, AF.hi);
    }
    break;
    case 0xE1:
        CPU_POP(HL);
        break;
    case 0xE2: // TODO
    {
        writeByte((0xFF00 + BC.lo), AF.hi);
    }
    break;
    // case 0xE3: // TODO: None?
    // case 0xE4: // TODO: also none
    case 0xE5:
        CPU_PUSH(HL);
        break;
    case 0xE6: // TODO: Check
    {
        uint8_t op = readByte(pc);
        CPU_8BIT_AND(AF.hi, op, true);
    }
    break;
    case 0xE7: // TODO: RST 20H
        CPU_RST(0x20);
        break;
    case 0xE8: // TODO: check
    {
        uint8_t n = readByte(pc++);
        CPU_16BIT_ADD(sp, n);
    }
    break;
    case 0xE9:
        CPU_JP(HL.value);
        break;
    case 0xEA: // TODO: LD [addr], A
    {
        uint16_t nn = readWord(pc);
        /*
        nn = nn << 8;
        nn |= readWord(pc); */
        pc += 2;
        writeByte(nn, AF.hi);
    }
    break;
    // case 0xEB: // TODO: None for EB, EC, ED
    // case 0xEC:
    // case 0xED:
    case 0xEE: // TODO: check
    {
        uint8_t op = readByte(pc);
        CPU_8BIT_XOR(AF.hi, op, true);
    }
    break;
    case 0xEF: // TODO: RST 28H
        CPU_RST(0x28);
        break;
    case 0xF0:
    {
        /*
        uint8_t n = readByte(pc++); // TODO: check
        // pc++;
        CPU_REG_ROM_LD(AF.hi, (0xFF00 + n));*/
        uint8_t n = readByte(pc++);
        uint16_t addr = 0xFF00 + n;
        AF.hi = readByte(addr);
    }
    break;
    case 0xF1:
        CPU_POP(AF);
        break;
    case 0xF2: // TODO: check correctness
        CPU_REG_ROM_LD(AF.hi, (0xFF00 + BC.lo));
        break;
    case 0xF3: // DI, aka disable interrupt
        CPU_DI();
        break;
    // case 0xF4: // TODO: None?
    case 0xF5:
        CPU_PUSH(AF);
        break;
    case 0xF6:
    {
        uint8_t n = readByte(pc);
        pc++;
        CPU_8BIT_OR(AF.hi, n, true);
    }
    break;
    case 0xF7: // TODO: RST 30H
        CPU_RST(0x30);
        break;
    case 0xF8: // TODO: LD HL, SP+n
    {
        uint8_t n = readByte(pc);
        pc++;
        CPU_16BIT_REG_LD(HL.value, sp + n);
    }
    break;
    case 0xF9: // TODO: check accuracy
        CPU_16BIT_REG_LD(sp, HL.value);
        break;
    case 0xFA: // TODO: LD A, [addr]
    {
        uint16_t nn = readWord(pc);
        pc += 2;
        uint8_t n = readByte(nn);
        AF.hi = n;
    }
    break;
    case 0xFB: // EI / Enable Interrupt
        CPU_EI();
        break;
    case 0xFE:
    {
        uint8_t n = readByte(pc++); // TODO check
        CPU_8BIT_CP(AF.hi, n);
    }
    break;
    case 0xFF: // TODO: RST 38H
        CPU_RST(0x38);
        break;
    case 0xFC: // illegal
    case 0xFD: // illegal
    default:
    {
        printf("Unsupported opcode: 0x%02x at 0x%04x\n", opcode, pc);
        exit(EXIT_FAILURE);
        pc++;
    }
    break;
    }
    if (ext_op)
        return op_ext_cycles[opcode];
    else
        return op_cycles[opcode];
}

int CPU::tick()
{
    int cycles = 0;

    if (!cpuHalted)
    {
        if (pendingEnable)
        { // TODO: rework this code
            mmu->enableInterrupts(true);
            pendingEnable = false;
        }
        else if (pendingDisable)
        {
            mmu->enableInterrupts(false);
            pendingDisable = false;
        }
        uint8_t opcode = fetch();
        cycles = execute(opcode);
    }
    else
        cycles = 4;
    return cycles;
}

void CPU::executeExtendedOpcode()
{
    uint8_t op = readByte(pc++); // TODO: Test for correctness
    // printf("ExtOpcode: CB%.2X\n", op);
    //   pc += 2;
    switch (op)
    {
    // RLC n
    case 0x00:
        CPU_RLC(BC.hi);
        break;
    case 0x01:
        CPU_RLC(BC.lo);
        break;
    case 0x02:
        CPU_RLC(DE.hi);
        break;
    case 0x03:
        CPU_RLC(DE.lo);
        break;
    case 0x04:
        CPU_RLC(HL.hi);
        break;
    case 0x05:
        CPU_RLC(HL.lo);
        break;
    case 0x06:
        CPU_RLC_MEM(HL.value);
        break;
    case 0x07:
        CPU_RLC(AF.hi);
        break;
    // RL n
    case 0x10:
        CPU_RL(BC.hi);
        break;
    case 0x11:
        CPU_RL(BC.lo);
        break;
    case 0x12:
        CPU_RL(DE.hi);
        break;
    case 0x13:
        CPU_RL(DE.lo);
        break;
    case 0x14:
        CPU_RL(HL.hi);
        break;
    case 0x15:
        CPU_RL(HL.lo);
        break;
    case 0x16:
        CPU_RL_MEM(HL.value);
        break;
    case 0x17:
        CPU_RL(AF.hi);
        break;
    // RRC n
    case 0x08:
        CPU_RRC(BC.hi);
        break;
    case 0x09:
        CPU_RRC(BC.lo);
        break;
    case 0x0A:
        CPU_RRC(DE.hi);
        break;
    case 0x0B:
        CPU_RRC(DE.lo);
        break;
    case 0x0C:
        CPU_RRC(HL.hi);
        break;
    case 0x0D:
        CPU_RRC(HL.lo);
        break;
    case 0x0E:
        CPU_RRC_MEM(HL.value);
        break;
    case 0x0F:
        CPU_RRC(AF.hi);
        break;
    // RR n
    case 0x18:
        CPU_RR(BC.hi);
        break;
    case 0x19:
        CPU_RR(BC.lo);
        break;
    case 0x1A:
        CPU_RR(DE.hi);
        break;
    case 0x1B:
        CPU_RR(DE.lo);
        break;
    case 0x1C:
        CPU_RR(HL.hi);
        break;
    case 0x1D:
        CPU_RR(HL.lo);
        break;
    case 0x1E:
        CPU_RR_MEM(HL.value);
        break;
    case 0x1F:
        CPU_RR(AF.hi);
        break;
    // SLA n
    case 0x27:
        CPU_SLA(AF.hi);
        break;
    case 0x20:
        CPU_SLA(BC.hi);
        break;
    case 0x21:
        CPU_SLA(BC.lo);
        break;
    case 0x22:
        CPU_SLA(DE.hi);
        break;
    case 0x23:
        CPU_SLA(DE.lo);
        break;
    case 0x24:
        CPU_SLA(HL.hi);
        break;
    case 0x25:
        CPU_SLA_MEM(HL.lo);
        break;
    case 0x26:
        CPU_SLA_MEM(HL.value);
        break;
    // SRA n
    case 0x2F:
        CPU_SRA(AF.hi);
        break;
    case 0x28:
        CPU_SRA(BC.hi);
        break;
    case 0x29:
        CPU_SRA(BC.lo);
        break;
    case 0x2A:
        CPU_SRA(DE.hi);
        break;
    case 0x2B:
        CPU_SRA(DE.lo);
        break;
    case 0x2C:
        CPU_SRA(HL.hi);
        break;
    case 0x2D:
        CPU_SRA(HL.lo);
        break;
    case 0x2E:
        CPU_SRA_MEM(HL.value);
        break;
    // SWAP n
    case 0x30:
        CPU_SWAP(BC.hi);
        break;
    case 0x31:
        CPU_SWAP(BC.lo);
        break;
    case 0x32:
        CPU_SWAP(DE.hi);
        break;
    case 0x33:
        CPU_SWAP(DE.lo);
        break;
    case 0x34:
        CPU_SWAP(HL.hi);
        break;
    case 0x35:
        CPU_SWAP(HL.lo);
        break;
    case 0x36:
    {
        uint8_t n = readByte(HL.value);
        CPU_SWAP(n);
        writeByte(HL.value, n);
    }
    case 0x37:
        CPU_SWAP(AF.hi);
        break;
    // SRL n
    case 0x3F:
        CPU_SRL(AF.hi);
        break;
    case 0x38:
        CPU_SRL(BC.hi);
        break;
    case 0x39:
        CPU_SRL(BC.lo);
        break;
    case 0x3A:
        CPU_SRL(DE.hi);
        break;
    case 0x3B:
        CPU_SRL(DE.lo);
        break;
    case 0x3C:
        CPU_SRL(HL.hi);
        break;
    case 0x3D:
        CPU_SRL(HL.lo);
        break;
    case 0x3E:
        CPU_SRL_MEM(HL.value);
        break;
    // BIT b,r
    case 0x47:
        CPU_BIT(AF.hi, 0);
        break;
    case 0x40:
        CPU_BIT(BC.hi, 0);
        break;
    case 0x41:
        CPU_BIT(BC.lo, 0);
        break;
    case 0x42:
        CPU_BIT(DE.hi, 0);
        break;
    case 0x43:
        CPU_BIT(DE.lo, 0);
        break;
    case 0x44:
        CPU_BIT(HL.hi, 0);
        break;
    case 0x45:
        CPU_BIT(HL.lo, 0);
        break;
    case 0x46:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 0);
    }
    break;
    case 0x48:
        CPU_BIT(BC.hi, 1);
        break;
    case 0x49:
        CPU_BIT(BC.lo, 1);
        break;
    case 0x4A:
        CPU_BIT(DE.hi, 1);
        break;
    case 0x4B:
        CPU_BIT(DE.lo, 1);
        break;
    case 0x4C:
        CPU_BIT(HL.hi, 1);
        break;
    case 0x4D:
        CPU_BIT(HL.lo, 1);
        break;
    case 0x4E:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 1);
    }
    break;
    case 0x4F:
        CPU_BIT(AF.hi, 1);
        break;
    case 0x50:
        CPU_BIT(BC.hi, 2);
        break;
    case 0x51:
        CPU_BIT(BC.lo, 2);
        break;
    case 0x52:
        CPU_BIT(DE.hi, 2);
        break;
    case 0x53:
        CPU_BIT(DE.lo, 2);
        break;
    case 0x54:
        CPU_BIT(HL.hi, 2);
        break;
    case 0x55:
        CPU_BIT(HL.lo, 2);
        break;
    case 0x56:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 2);
    }
    break;
    case 0x57:
        CPU_BIT(AF.hi, 2);
    case 0x58:
        CPU_BIT(BC.hi, 3);
        break;
    case 0x59:
        CPU_BIT(BC.lo, 3);
        break;
    case 0x5A:
        CPU_BIT(DE.hi, 3);
        break;
    case 0x5B:
        CPU_BIT(DE.lo, 3);
        break;
    case 0x5C:
        CPU_BIT(HL.hi, 3);
        break;
    case 0x5D:
        CPU_BIT(HL.lo, 3);
        break;
    case 0x5E:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 3);
    }
    break;
    case 0x5F:
        CPU_BIT(AF.hi, 3);
        break;
    case 0x60:
        CPU_BIT(BC.hi, 4);
        break;
    case 0x61:
        CPU_BIT(BC.lo, 4);
        break;
    case 0x62:
        CPU_BIT(DE.hi, 4);
        break;
    case 0x63:
        CPU_BIT(DE.lo, 4);
        break;
    case 0x64:
        CPU_BIT(HL.hi, 4);
        break;
    case 0x65:
        CPU_BIT(HL.lo, 4);
        break;
    case 0x66:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 4);
    }
    break;
    case 0x67:
        CPU_BIT(AF.hi, 4);
    case 0x68:
        CPU_BIT(BC.hi, 5);
        break;
    case 0x69:
        CPU_BIT(BC.lo, 5);
        break;
    case 0x6A:
        CPU_BIT(DE.hi, 5);
        break;
    case 0x6B:
        CPU_BIT(DE.lo, 5);
        break;
    case 0x6C:
        CPU_BIT(HL.hi, 5);
        break;
    case 0x6D:
        CPU_BIT(HL.lo, 5);
        break;
    case 0x6E:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 5);
    }
    break;
    case 0x6F:
        CPU_BIT(AF.hi, 5);
        break;
    case 0x70:
        CPU_BIT(BC.hi, 6);
        break;
    case 0x71:
        CPU_BIT(BC.lo, 6);
        break;
    case 0x72:
        CPU_BIT(DE.hi, 6);
        break;
    case 0x73:
        CPU_BIT(DE.lo, 6);
        break;
    case 0x74:
        CPU_BIT(HL.hi, 6);
        break;
    case 0x75:
        CPU_BIT(HL.lo, 6);
        break;
    case 0x76:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 6);
    }
    break;
    case 0x77:
        CPU_BIT(AF.hi, 6);
    case 0x78:
        CPU_BIT(BC.hi, 7);
        break;
    case 0x79:
        CPU_BIT(BC.lo, 7);
        break;
    case 0x7A:
        CPU_BIT(DE.hi, 7);
        break;
    case 0x7B:
        CPU_BIT(DE.lo, 7);
        break;
    case 0x7C:
        CPU_BIT(HL.hi, 7);
        break;
    case 0x7D:
        CPU_BIT(HL.lo, 7);
        break;
    case 0x7E:
    {
        uint8_t n = readByte(HL.value);
        CPU_BIT(n, 7);
    }
    break;
    case 0x7F:
        CPU_BIT(AF.hi, 7);
        break;
    // SET b,r
    case 0xC7:
        CPU_SET(AF.hi, 0);
        break;
    case 0xC0:
        CPU_SET(BC.hi, 0);
        break;
    case 0xC1:
        CPU_SET(BC.lo, 0);
        break;
    case 0xC2:
        CPU_SET(DE.hi, 0);
        break;
    case 0xC3:
        CPU_SET(DE.lo, 0);
        break;
    case 0xC4:
        CPU_SET(HL.hi, 0);
        break;
    case 0xC5:
        CPU_SET(HL.lo, 0);
        break;
    case 0xC6:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 0);
        writeByte(HL.value, n);
    }
    break;
    case 0xC8:
        CPU_SET(BC.hi, 1);
        break;
    case 0xC9:
        CPU_SET(BC.lo, 1);
        break;
    case 0xCA:
        CPU_SET(DE.hi, 1);
        break;
    case 0xCB:
        CPU_SET(DE.lo, 1);
        break;
    case 0xCC:
        CPU_SET(HL.hi, 1);
        break;
    case 0xCD:
        CPU_SET(HL.lo, 1);
        break;
    case 0xCE:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 1);
        writeByte(HL.value, n);
    }
    break;
    case 0xCF:
        CPU_SET(AF.hi, 1);
        break;
    case 0xD0:
        CPU_SET(BC.hi, 2);
        break;
    case 0xD1:
        CPU_SET(BC.lo, 2);
        break;
    case 0xD2:
        CPU_SET(DE.hi, 2);
        break;
    case 0xD3:
        CPU_SET(DE.lo, 2);
        break;
    case 0xD4:
        CPU_SET(HL.hi, 2);
        break;
    case 0xD5:
        CPU_SET(HL.lo, 2);
        break;
    case 0xD6:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 2);
        writeByte(HL.value, n);
    }
    break;
    case 0xD7:
        CPU_SET(AF.hi, 2);
        break;
    case 0xD8:
        CPU_SET(BC.hi, 3);
        break;
    case 0xD9:
        CPU_SET(BC.lo, 3);
        break;
    case 0xDA:
        CPU_SET(DE.hi, 3);
        break;
    case 0xDB:
        CPU_SET(DE.lo, 3);
        break;
    case 0xDC:
        CPU_SET(HL.hi, 3);
        break;
    case 0xDD:
        CPU_SET(HL.lo, 3);
        break;
    case 0xDE:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 3);
        writeByte(HL.value, n);
    }
    break;
    case 0xDF:
        CPU_SET(AF.hi, 3);
        break;
    case 0xE0:
        CPU_SET(BC.hi, 4);
        break;
    case 0xE1:
        CPU_SET(BC.lo, 4);
        break;
    case 0xE2:
        CPU_SET(DE.hi, 4);
        break;
    case 0xE3:
        CPU_SET(DE.lo, 4);
        break;
    case 0xE4:
        CPU_SET(HL.hi, 4);
        break;
    case 0xE5:
        CPU_SET(HL.lo, 4);
        break;
    case 0xE6:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 4);
        writeByte(HL.value, n);
    }
    break;
    case 0xE7:
        CPU_SET(AF.hi, 4);
        break;
    case 0xE8:
        CPU_SET(BC.hi, 5);
        break;
    case 0xE9:
        CPU_SET(BC.lo, 5);
        break;
    case 0xEA:
        CPU_SET(DE.hi, 5);
        break;
    case 0xEB:
        CPU_SET(DE.lo, 5);
        break;
    case 0xEC:
        CPU_SET(HL.hi, 5);
        break;
    case 0xED:
        CPU_SET(HL.lo, 5);
        break;
    case 0xEE:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 5);
        writeByte(HL.value, n);
    }
    break;
    case 0xEF:
        CPU_SET(AF.hi, 5);
        break;
    case 0xF0:
        CPU_SET(BC.hi, 6);
        break;
    case 0xF1:
        CPU_SET(BC.lo, 6);
        break;
    case 0xF2:
        CPU_SET(DE.hi, 6);
        break;
    case 0xF3:
        CPU_SET(DE.lo, 6);
        break;
    case 0xF4:
        CPU_SET(HL.hi, 6);
        break;
    case 0xF5:
        CPU_SET(HL.lo, 6);
        break;
    case 0xF6:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 6);
        writeByte(HL.value, n);
    }
    break;
    case 0xF7:
        CPU_SET(AF.hi, 6);
        break;
    case 0xF8:
        CPU_SET(BC.hi, 7);
        break;
    case 0xF9:
        CPU_SET(BC.lo, 7);
        break;
    case 0xFA:
        CPU_SET(DE.hi, 7);
        break;
    case 0xFB:
        CPU_SET(DE.lo, 7);
        break;
    case 0xFC:
        CPU_SET(HL.hi, 7);
        break;
    case 0xFD:
        CPU_SET(HL.lo, 7);
        break;
    case 0xFE:
    {
        uint8_t n = readByte(HL.value);
        CPU_SET(n, 7);
        writeByte(HL.value, n);
    }
    break;
    case 0xFF:
        CPU_SET(AF.hi, 7);
        break;
    // TODO: RES b,r
    case 0x87:
        CPU_RES(AF.hi, 0);
        break;
    case 0x80:
        CPU_RES(BC.hi, 0);
        break;
    case 0x81:
        CPU_RES(BC.lo, 0);
        break;
    case 0x82:
        CPU_RES(DE.hi, 0);
        break;
    case 0x83:
        CPU_RES(DE.lo, 0);
        break;
    case 0x84:
        CPU_RES(HL.hi, 0);
        break;
    case 0x85:
        CPU_RES(HL.lo, 0);
        break;
    case 0x86:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 0);
        writeByte(HL.value, n);
    }
    break;
    case 0x88:
        CPU_RES(BC.hi, 1);
        break;
    case 0x89:
        CPU_RES(BC.lo, 1);
        break;
    case 0x8A:
        CPU_RES(DE.hi, 1);
        break;
    case 0x8B:
        CPU_RES(DE.lo, 1);
        break;
    case 0x8C:
        CPU_RES(HL.hi, 1);
        break;
    case 0x8D:
        CPU_RES(HL.lo, 1);
        break;
    case 0x8E:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 1);
        writeByte(HL.value, n);
    }
    break;
    case 0x8F:
        CPU_RES(AF.hi, 1);
        break;
    case 0x90: // TODO: check
        CPU_RES(BC.hi, 2);
        break;
    case 0x91:
        CPU_RES(BC.lo, 2);
        break;
    case 0x92:
        CPU_RES(DE.hi, 2);
        break;
    case 0x93:
        CPU_RES(DE.lo, 2);
        break;
    case 0x94:
        CPU_RES(HL.hi, 2);
        break;
    case 0x95:
        CPU_RES(HL.lo, 2);
        break;
    case 0x96:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 2);
        writeByte(HL.value, n);
    }
    break;
    case 0x97:
        CPU_RES(AF.hi, 2);
        break;
    case 0x98:
        CPU_RES(BC.hi, 3);
        break;
    case 0x99:
        CPU_RES(BC.lo, 3);
        break;
    case 0x9A:
        CPU_RES(DE.hi, 3);
        break;
    case 0x9B:
        CPU_RES(DE.lo, 3);
        break;
    case 0x9C:
        CPU_RES(HL.hi, 3);
        break;
    case 0x9D:
        CPU_SET(HL.lo, 3);
        break;
    case 0x9E:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 3);
        writeByte(HL.value, n);
    }
    break;
    case 0x9F:
        CPU_RES(AF.hi, 3);
        break;
    case 0xA0:
        CPU_RES(BC.hi, 4);
        break;
    case 0xA1:
        CPU_RES(BC.lo, 4);
        break;
    case 0xA2:
        CPU_RES(DE.hi, 4);
        break;
    case 0xA3:
        CPU_RES(DE.lo, 4);
        break;
    case 0xA4:
        CPU_RES(HL.hi, 4);
        break;
    case 0xA5:
        CPU_RES(HL.lo, 4);
        break;
    case 0xA6:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 4);
        writeByte(HL.value, n);
    }
    break;
    case 0xA7:
        CPU_RES(AF.hi, 4);
        break;
    case 0xA8:
        CPU_RES(BC.hi, 5);
        break;
    case 0xA9:
        CPU_RES(BC.lo, 5);
        break;
    case 0xAA:
        CPU_RES(DE.hi, 5);
        break;
    case 0xAB:
        CPU_RES(DE.lo, 5);
        break;
    case 0xAC:
        CPU_RES(HL.hi, 5);
        break;
    case 0xAD:
        CPU_RES(HL.lo, 5);
        break;
    case 0xAE:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 5);
        writeByte(HL.value, n);
    }
    break;
    case 0xAF:
        CPU_RES(AF.hi, 5);
        break;
    case 0xB0:
        CPU_RES(BC.hi, 6);
        break;
    case 0xB1:
        CPU_RES(BC.lo, 6);
        break;
    case 0xB2:
        CPU_RES(DE.hi, 6);
        break;
    case 0xB3:
        CPU_RES(DE.lo, 6);
        break;
    case 0xB4:
        CPU_RES(HL.hi, 6);
        break;
    case 0xB5:
        CPU_RES(HL.lo, 6);
        break;
    case 0xB6:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 6);
        writeByte(HL.value, n);
    }
    break;
    case 0xB7:
        CPU_RES(AF.hi, 6);
        break;
    case 0xB8:
        CPU_RES(BC.hi, 7);
        break;
    case 0xB9:
        CPU_RES(BC.lo, 7);
        break;
    case 0xBA:
        CPU_RES(DE.hi, 7);
        break;
    case 0xBB:
        CPU_RES(DE.lo, 7);
        break;
    case 0xBC:
        CPU_RES(HL.hi, 7);
        break;
    case 0xBD:
        CPU_RES(HL.lo, 7);
        break;
    case 0xBE:
    {
        uint8_t n = readByte(HL.value);
        CPU_RES(n, 7);
        writeByte(HL.value, n);
    }
    break;
    case 0xBF:
        CPU_RES(AF.hi, 7);
        break;
    default:
        printf("Unsupported opcode: 0xCB%02x at 0x%04x\n\n\n", op, pc);
        exit(EXIT_FAILURE);
        // pc++;
    }
}