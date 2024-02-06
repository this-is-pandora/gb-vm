#include "../include/cpu.h"

CPU::CPU(MMU *mmu)
{
    // boot rom
    AF.value = 0x01B0;
    BC.value = 0x0013;
    DE.value = 0x00D8;
    HL.value = 0x014D;

    clockCycles = 0;
    clockSpeed = 1024;

    accessedMemory = false;
    cpuStopped = false;
    cpuHalted = false;
    interruptsEnabled = false;
    debugMode = false;

    memory = mmu;
}

void CPU::writeByte(uint16_t addr, uint8_t data)
{
    memory->writeByte(addr, data);
}

void CPU::writeWord(uint16_t addr, uint16_t data)
{
    memory->writeWord(addr, data);
}

uint8_t CPU::readByte(uint16_t addr)
{
    uint8_t data = memory->readByte(addr);
    return data;
}

uint16_t CPU::readWord(uint16_t addr)
{
    uint16_t data = memory->readWord(addr);
    return data;
}

bool CPU::getZeroFlag() const
{
    return (AF.lo & FLAG_MASK_Z) ? true : false;
}
bool CPU::getSubtractFlag() const
{
    return (AF.lo & FLAG_MASK_N) ? true : false;
}

bool CPU::getCarryFlag() const
{
    return (AF.lo & FLAG_MASK_C) ? true : false;
}

bool CPU::getHalfCarryFlag() const
{
    return (AF.lo & FLAG_MASK_H) ? true : false;
}

uint8_t CPU::fetch()
{
    return memory->readByte(pc++);
    // return m_ROM[pc++];
}

int CPU::execute(uint8_t opcode)
{
    switch (opcode)
    {
    case 0x00: // NOP
        CPU_NOP;
        break;
    case 0x01: // ld bc, imm
        CPU_16BIT_LD(BC.value, op_cycles[opcode]);
        break;
    case 0x02: // ld (bc), a
        CPU_ROM_REG_LD(BC.value, AF.hi, op_cycles[opcode]);
        break;
    case 0x03:
        CPU_16BIT_INC(BC.value, op_cycles[opcode]);
        break;
    case 0x04:
        CPU_8BIT_INC(BC.hi, op_cycles[opcode]);
    case 0x05:
        CPU_8BIT_DEC(BC.hi, op_cycles[opcode]);
        break;
    case 0x06:
        CPU_8BIT_LD(BC.hi, op_cycles[opcode]);
        break;
    case 0x07: // RLCA
        CPU_RLCA();
        break;
    case 0x08: // TODO: LD nn, sp
    {
        /*
            uint16_t nn = readWord(++pc);
            nn = nn << 8;
            nn |= readWord(pc);
            pc += 2;
            nn++;
            CPU_16BIT_ROM_LD(nn, sp, op_cycles[opcode]); */
        uint16_t nn = readWord(pc);
        pc += 2;
        writeByte(nn++, sp);
        writeByte(nn, sp);
    }
    break;
    case 0x09:
        CPU_16BIT_ADD(HL.value, BC.value, op_cycles[opcode]);
        break;
    case 0x0A:
        CPU_REG_ROM_LD(AF.hi, BC.value, op_cycles[opcode]);
        break;
    case 0x0B:
        CPU_16BIT_DEC(BC.value, op_cycles[opcode]);
        break;
    case 0x0C:
        CPU_8BIT_INC(BC.lo, op_cycles[opcode]);
        break;
    case 0x0D:
        CPU_8BIT_DEC(BC.lo, op_cycles[opcode]);
        break;
    case 0x0E:
        CPU_8BIT_LD(BC.lo, op_cycles[opcode]);
        break;
    case 0x0F:
        CPU_RRCA();
        break;
    case 0x10: // TODO stop
        // state = STOPPED;
        CPU_STOP();
        break;
    case 0x11: // LD DE, nn
        CPU_16BIT_LD(DE.value, op_cycles[opcode]);
        break;
    case 0x12: // LD (DE), A
        CPU_ROM_REG_LD(DE.value, AF.hi, op_cycles[opcode]);
        break;
    case 0x13:
        CPU_16BIT_INC(DE.value, op_cycles[opcode]);
        break;
    case 0x14:
        CPU_8BIT_INC(DE.hi, op_cycles[opcode]);
        break;
    case 0x15:
        CPU_8BIT_DEC(DE.hi, op_cycles[opcode]);
        break;
    case 0x16:
        CPU_8BIT_LD(DE.hi, op_cycles[opcode]);
        break;
    case 0x17:
        CPU_RLA();
        break;
    case 0x18: // TODO: JR n
    {
        int8_t n = (int8_t)readByte(pc);
        CPU_JR(n);
    }
    break;
    case 0x19:
        CPU_16BIT_ADD(HL.value, DE.value, op_cycles[opcode]);
        break;
    case 0x1A:
        CPU_REG_ROM_LD(AF.hi, DE.value, op_cycles[opcode]);
        break;
    case 0x1B:
        CPU_16BIT_DEC(DE.value, op_cycles[opcode]);
        break;
    case 0x1C:
        CPU_8BIT_INC(DE.lo, op_cycles[opcode]);
        break;
    case 0x1D:
        CPU_8BIT_DEC(DE.lo, op_cycles[opcode]);
        break;
    case 0x1E:
        CPU_8BIT_LD(DE.lo, op_cycles[opcode]);
        break;
    case 0x1F:
        CPU_RRA();
        break;
    case 0x20: // TODO: JR NZ, *
    {
        int8_t n = (int8_t)readByte(pc);
        if (getZeroFlag() == false)
            CPU_JR(n);
        else
            pc++;
    }
    break;
    case 0x21:
        CPU_16BIT_LD(HL.value, op_cycles[opcode]);
    case 0x22:
    {
        CPU_ROM_REG_LD(HL.value, AF.hi, op_cycles[opcode]);
        CPU_16BIT_INC(HL.value, 0);
    }
    break;
    case 0x23:
        CPU_16BIT_INC(HL.value, op_cycles[opcode]);
        break;
    case 0x24:
        CPU_8BIT_INC(HL.hi, op_cycles[opcode]);
        break;
    case 0x25:
        CPU_8BIT_DEC(HL.hi, op_cycles[opcode]);
        break;
    case 0x26:
        CPU_8BIT_LD(HL.hi, op_cycles[opcode]);
        break;
    case 0x27: // decimal adjust register A (DAA)
        CPU_DAA();
        break;
    case 0x28: // JR Z,*
    {
        int8_t n = (int8_t)readByte(pc);
        if (getZeroFlag() == true)
            CPU_JR(n);
        else
            pc++;
    }
    break;
    case 0x29:
        CPU_16BIT_ADD(HL.value, HL.value, op_cycles[opcode]);
        break;
    case 0x2A:
    {
        CPU_REG_ROM_LD(AF.hi, HL.value, op_cycles[opcode]);
        CPU_16BIT_INC(HL.value, 0);
    }
    break;
    case 0x2B:
        CPU_16BIT_DEC(HL.value, op_cycles[opcode]);
        break;
    case 0x2C:
        CPU_8BIT_INC(HL.lo, op_cycles[opcode]);
        break;
    case 0x2D:
        CPU_8BIT_DEC(HL.lo, op_cycles[opcode]);
        break;
    case 0x2E:
        CPU_8BIT_LD(HL.lo, op_cycles[opcode]);
        break;
    case 0x2F:
        CPU_CPL();
        break;
    case 0x30: // TODO: JR NC,*
    {
        int8_t n = (int8_t)readByte(pc);
        if (getCarryFlag() == false)
            CPU_JR(n);
        else
            pc++;
    }
    break;
    case 0x31:
        CPU_16BIT_LD(sp, op_cycles[opcode]);
    case 0x32: // LD (HLD),A
    {
        CPU_ROM_REG_LD(HL.value, AF.hi, op_cycles[opcode]);
        CPU_16BIT_DEC(HL.value, 0);
    }
    break;
    case 0x33:
        CPU_16BIT_INC(sp, op_cycles[opcode]);
        break;
    case 0x34: // TODO: check
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_INC(n, op_cycles[opcode]);
    }
    break;
    case 0x35: // TODO: FIX
    {
        uint8_t n = readByte(HL.value);
        CPU_8BIT_DEC(n, op_cycles[opcode]);
    }
    break;
    case 0x36: // TODO
    {
        uint8_t n = readByte(pc);
        pc++;
        writeByte(HL.value, n);
        // clockCycles += op_cycles[opcode];
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
        CPU_16BIT_ADD(HL.value, sp, op_cycles[opcode]);
        break;
    case 0x3A: // LD A,(HLD)
    {
        CPU_REG_LD(AF.hi, HL.value, op_cycles[opcode]);
        CPU_16BIT_DEC(HL.value, 0);
    }
    break;
    case 0x3B:
        CPU_16BIT_DEC(sp, op_cycles[opcode]);
        break;
    case 0x3C:
        CPU_8BIT_INC(AF.hi, op_cycles[opcode]);
        break;
    case 0x3D:
        CPU_8BIT_DEC(AF.hi, op_cycles[opcode]);
        break;
    case 0x3E:
    {
        uint8_t n = readByte(pc);
        AF.hi = n;
        pc++;
        // clockCycles += op_cycles[opcode];
    }
    break;
    case 0x3F:
        CPU_CCF();
        break;
    case 0x40:
        CPU_REG_LD(BC.hi, BC.hi, op_cycles[opcode]);
        break;
    case 0x41:
        CPU_REG_LD(BC.hi, BC.lo, op_cycles[opcode]);
        break;
    case 0x42:
        CPU_REG_LD(BC.hi, DE.hi, op_cycles[opcode]);
        break;
    case 0x43:
        CPU_REG_LD(BC.hi, DE.lo, op_cycles[opcode]);
        break;
    case 0x44:
        CPU_REG_LD(BC.hi, HL.hi, op_cycles[opcode]);
        break;
    case 0x45:
        CPU_REG_LD(BC.hi, HL.lo, op_cycles[opcode]);
        break;
    case 0x46:
        CPU_REG_ROM_LD(BC.hi, HL.value, op_cycles[opcode]);
        break;
    case 0x47:
    case 0x48:
        CPU_REG_LD(BC.lo, BC.hi, op_cycles[opcode]);
        break;
    case 0x49:
        CPU_REG_LD(BC.lo, BC.lo, op_cycles[opcode]);
        break;
    case 0x4A:
        CPU_REG_LD(BC.lo, DE.hi, op_cycles[opcode]);
        break;
    case 0x4B:
        CPU_REG_LD(BC.lo, DE.lo, op_cycles[opcode]);
        break;
    case 0x4C:
        CPU_REG_LD(BC.lo, HL.hi, op_cycles[opcode]);
        break;
    case 0x4D:
        CPU_REG_LD(BC.lo, HL.lo, op_cycles[opcode]);
        break;
    case 0x4E:
        CPU_REG_ROM_LD(BC.lo, HL.value, op_cycles[opcode]); // ld C, (HL)
        break;
    case 0x4F: // LD C,A
        CPU_REG_LD(BC.lo, AF.hi, op_cycles[opcode]);
        break;
    case 0x50: // LD D,B
        CPU_REG_LD(DE.hi, BC.hi, op_cycles[opcode]);
        break;
    case 0x51:
        CPU_REG_LD(DE.hi, BC.lo, op_cycles[opcode]);
        break;
    case 0x52:
        CPU_REG_LD(DE.hi, DE.hi, op_cycles[opcode]);
        break;
    case 0x53:
        CPU_REG_LD(DE.hi, DE.lo, op_cycles[opcode]);
        break;
    case 0x54:
        CPU_REG_LD(DE.hi, HL.hi, op_cycles[opcode]);
        break;
    case 0x55:
        CPU_REG_LD(DE.hi, HL.lo, op_cycles[opcode]);
        break;
    case 0x56:
        CPU_REG_ROM_LD(DE.hi, HL.value, op_cycles[opcode]);
        break;
    case 0x57:
        CPU_REG_LD(DE.hi, AF.hi, op_cycles[opcode]);
        break;
    case 0x58:
        CPU_REG_LD(DE.lo, BC.hi, op_cycles[opcode]);
        break;
    case 0x59:
        CPU_REG_LD(DE.lo, BC.lo, op_cycles[opcode]);
        break;
    case 0x5A:
        CPU_REG_LD(DE.lo, DE.hi, op_cycles[opcode]);
        break;
    case 0x5B:
        CPU_REG_LD(DE.lo, DE.lo, op_cycles[opcode]);
        break;
    case 0x5C:
        CPU_REG_LD(DE.lo, HL.hi, op_cycles[opcode]);
        break;
    case 0x5D:
        CPU_REG_LD(DE.lo, HL.lo, op_cycles[opcode]);
        break;
    case 0x5E:
        CPU_REG_ROM_LD(DE.lo, HL.value, op_cycles[opcode]);
        break;
    case 0x5F: // LD E,A
        CPU_REG_LD(DE.lo, AF.hi, op_cycles[opcode]);
        break;
    case 0x60: // LD H,B
        CPU_REG_LD(HL.hi, BC.hi, op_cycles[opcode]);
        break;
    case 0x61: // LD H,C
        CPU_REG_LD(HL.hi, BC.lo, op_cycles[opcode]);
        break;
    case 0x62: // LD H,D
        CPU_REG_LD(HL.hi, DE.hi, op_cycles[opcode]);
        break;
    case 0x63: // LD H,E
        CPU_REG_LD(HL.hi, DE.lo, op_cycles[opcode]);
        break;
    case 0x64: // LD H,H
        CPU_REG_LD(HL.hi, HL.hi, op_cycles[opcode]);
        break;
    case 0x65:
        CPU_REG_LD(HL.hi, HL.lo, op_cycles[opcode]);
        break;
    case 0x66:
        CPU_REG_ROM_LD(HL.hi, HL.value, op_cycles[opcode]);
        break;
    case 0x67: // LD H,A
        CPU_REG_LD(HL.hi, AF.hi, op_cycles[opcode]);
        break;
    case 0x68:
        CPU_REG_LD(HL.lo, BC.hi, op_cycles[opcode]);
        break;
    case 0x69:
        CPU_REG_LD(HL.lo, BC.lo, op_cycles[opcode]);
        break;
    case 0x6A:
        CPU_REG_LD(HL.lo, DE.hi, op_cycles[opcode]);
        break;
    case 0x6B:
        CPU_REG_LD(HL.lo, DE.lo, op_cycles[opcode]);
        break;
    case 0x6C:
        CPU_REG_LD(HL.lo, HL.hi, op_cycles[opcode]);
        break;
    case 0x6D:
        CPU_REG_LD(HL.lo, HL.lo, op_cycles[opcode]);
        break;
    case 0x6E:
        CPU_REG_ROM_LD(HL.lo, HL.value, op_cycles[opcode]);
        break;
    case 0x6F: // LD L,A
        CPU_REG_LD(HL.lo, AF.hi, op_cycles[opcode]);
        break;
    case 0x70: // LD (HL),B
        CPU_ROM_REG_LD(HL.value, BC.hi, op_cycles[opcode]);
        break;
    case 0x71:
        CPU_ROM_REG_LD(HL.value, BC.lo, op_cycles[opcode]);
        break;
    case 0x72:
        CPU_ROM_REG_LD(HL.value, DE.hi, op_cycles[opcode]);
        break;
    case 0x73:
        CPU_ROM_REG_LD(HL.value, DE.lo, op_cycles[opcode]);
        break;
    case 0x74:
        CPU_ROM_REG_LD(HL.value, HL.hi, op_cycles[opcode]);
        break;
    case 0x75:
        CPU_ROM_REG_LD(HL.value, HL.lo, op_cycles[opcode]);
        break;
    case 0x76: // TODO halt
        // state = HALTED;
        CPU_HALT();
        break;
    case 0x77:
        CPU_ROM_REG_LD(HL.value, AF.hi, op_cycles[opcode]);
        break;
    case 0x78:
        CPU_REG_LD(AF.hi, BC.hi, op_cycles[opcode]);
        break;
    case 0x79:
        CPU_REG_LD(AF.hi, BC.lo, op_cycles[opcode]);
        break;
    case 0x7A:
        CPU_REG_LD(AF.hi, DE.hi, op_cycles[opcode]);
        break;
    case 0x7B:
        CPU_REG_LD(AF.hi, DE.lo, op_cycles[opcode]);
        break;
    case 0x7C:
        CPU_REG_LD(AF.hi, HL.hi, op_cycles[opcode]);
        break;
    case 0x7D:
        CPU_REG_LD(AF.hi, HL.lo, op_cycles[opcode]);
        break;
    case 0x7E:
        CPU_REG_ROM_LD(AF.hi, HL.value, op_cycles[opcode]);
        break;
    case 0x7F:
        CPU_REG_LD(AF.hi, AF.hi, op_cycles[opcode]);
        break;
    case 0x80:
        CPU_8BIT_ADD(AF.hi, BC.hi, op_cycles[opcode], false, false);
        break;
    case 0x81:
        CPU_8BIT_ADD(AF.hi, BC.lo, op_cycles[opcode], false, false);
        break;
    case 0x82:
        CPU_8BIT_ADD(AF.hi, DE.hi, op_cycles[opcode], false, false);
        break;
    case 0x83:
        CPU_8BIT_ADD(AF.hi, DE.lo, op_cycles[opcode], false, false);
        break;
    case 0x84:
        CPU_8BIT_ADD(AF.hi, HL.hi, op_cycles[opcode], false, false);
        break;
    case 0x85:
        CPU_8BIT_ADD(AF.hi, HL.lo, op_cycles[opcode], false, false);
        break;
    case 0x86: // TODO: ADD A, (HL)
        uint8_t n = readByte(HL.value);
        CPU_8BIT_ADD(AF.hi, n, op_cycles[opcode], false, false);
        break;
    case 0x87:
        CPU_8BIT_ADD(AF.hi, AF.hi, op_cycles[opcode], false, false);
        break;
    case 0x88:
        CPU_8BIT_ADC(BC.hi, op_cycles[opcode]);
        break;
    case 0x89:
        CPU_8BIT_ADC(BC.lo, op_cycles[opcode]);
        break;
    case 0x8A:
        CPU_8BIT_ADC(DE.hi, op_cycles[opcode]);
        break;
    case 0x8B:
        CPU_8BIT_ADC(DE.lo, op_cycles[opcode]);
        break;
    case 0x8C:
        CPU_8BIT_ADC(HL.hi, op_cycles[opcode]);
        break;
    case 0x8D:
        CPU_8BIT_ADC(HL.lo, op_cycles[opcode]);
        break;
    case 0x8E: // TODO: ADC A, (HL)
        uint8_t n = readByte(HL.value);
        CPU_8BIT_ADC(n, op_cycles[opcode]);
        break;
    case 0x8F:
        CPU_8BIT_ADC(AF.hi, op_cycles[opcode]);
        break;
    case 0x90:
        CPU_8BIT_SUB(AF.hi, BC.hi, op_cycles[opcode], false, false);
        break;
    case 0x91:
        CPU_8BIT_SUB(AF.hi, BC.lo, op_cycles[opcode], false, false);
        break;
    case 0x92:
        CPU_8BIT_SUB(AF.hi, DE.hi, op_cycles[opcode], false, false);
        break;
    case 0x93:
        CPU_8BIT_SUB(AF.hi, DE.lo, op_cycles[opcode], false, false);
        break;
    case 0x94:
        CPU_8BIT_SUB(AF.hi, HL.hi, op_cycles[opcode], false, false);
        break;
    case 0x95:
        CPU_8BIT_SUB(AF.hi, HL.lo, op_cycles[opcode], false, false);
        break;
    case 0x96:
        uint8_t n = readByte(HL.value);
        CPU_8BIT_SUB(AF.hi, n, op_cycles[opcode], false, false);
        break;
    case 0x97:
        CPU_8BIT_SUB(AF.hi, AF.hi, op_cycles[opcode], false, false);
        break;
    case 0x98:
        CPU_SBC(BC.hi, op_cycles[opcode]);
        break;
    case 0x99:
        CPU_SBC(BC.lo, op_cycles[opcode]);
        break;
    case 0x9A:
        CPU_SBC(DE.hi, op_cycles[opcode]);
        break;
    case 0x9B:
        CPU_SBC(DE.lo, op_cycles[opcode]);
        break;
    case 0x9C:
        CPU_SBC(HL.hi, op_cycles[opcode]);
        break;
    case 0x9D:
        CPU_SBC(HL.lo, op_cycles[opcode]);
        break;
    case 0x9E:
        uint8_t n = readByte(HL.value);
        CPU_SBC(n, op_cycles[opcode]);
        break;
    case 0x9F:
        CPU_SBC(AF.hi, op_cycles[opcode]);
        break;
    case 0xA0:
        CPU_8BIT_AND(AF.hi, BC.hi, op_cycles[opcode], false);
        break;
    case 0xA1:
        CPU_8BIT_AND(AF.hi, BC.lo, op_cycles[opcode], false);
        break;
    case 0xA2:
        CPU_8BIT_AND(AF.hi, DE.hi, op_cycles[opcode], false);
        break;
    case 0xA3:
        CPU_8BIT_AND(AF.hi, DE.lo, op_cycles[opcode], false);
        break;
    case 0xA4:
        CPU_8BIT_AND(AF.hi, HL.hi, op_cycles[opcode], false);
        break;
    case 0xA5:
        CPU_8BIT_AND(AF.hi, HL.lo, op_cycles[opcode], false);
        break;
    case 0xA6:
        uint8_t n = readByte(HL.value);
        CPU_8BIT_AND(AF.hi, n, op_cycles[opcode], false);
        break;
    case 0xA7: // TODO: fix bug
        CPU_8BIT_AND(AF.hi, AF.hi, op_cycles[opcode], false);
        break;
    case 0xA8:
        CPU_8BIT_XOR(AF.hi, BC.hi, op_cycles[opcode], false);
        break;
    case 0xA9:
        CPU_8BIT_XOR(AF.hi, BC.lo, op_cycles[opcode], false);
        break;
    case 0xAA:
        CPU_8BIT_XOR(AF.hi, DE.hi, op_cycles[opcode], false);
        break;
    case 0xAB:
        CPU_8BIT_XOR(AF.hi, DE.lo, op_cycles[opcode], false);
        break;
    case 0xAC:
        CPU_8BIT_XOR(AF.hi, HL.hi, op_cycles[opcode], false);
        break;
    case 0xAD:
        CPU_8BIT_XOR(AF.hi, HL.lo, op_cycles[opcode], false);
        break;
    case 0xAE:
        uint8_t n = readByte(HL.value);
        CPU_8BIT_XOR(AF.hi, n, op_cycles[opcode], false);
        break;
    case 0xAF:
        CPU_8BIT_XOR(AF.hi, AF.hi, op_cycles[opcode], false);
        break;
    case 0xB0:
        CPU_8BIT_OR(AF.hi, BC.hi, op_cycles[opcode], false);
        break;
    case 0xB1:
        CPU_8BIT_OR(AF.hi, BC.lo, op_cycles[opcode], false);
        break;
    case 0xB2:
        CPU_8BIT_OR(AF.hi, DE.hi, op_cycles[opcode], false);
        break;
    case 0xB3:
        CPU_8BIT_OR(AF.hi, DE.lo, op_cycles[opcode], false);
        break;
    case 0xB4:
        CPU_8BIT_OR(AF.hi, HL.hi, op_cycles[opcode], false);
        break;
    case 0xB5:
        CPU_8BIT_OR(AF.hi, HL.lo, op_cycles[opcode], false);
        break;
    case 0xB6:
        uint8_t n = readByte(HL.value);
        CPU_8BIT_OR(AF.hi, n, op_cycles[opcode], false);
        break;
    case 0xB7:
        CPU_8BIT_OR(AF.hi, AF.hi, op_cycles[opcode], false);
        break;
    case 0xB8:
        CPU_8BIT_CP(AF.hi, BC.hi, op_cycles[opcode]);
    case 0xB9:
        CPU_8BIT_CP(AF.hi, BC.lo, op_cycles[opcode]);
    case 0xBA:
        CPU_8BIT_CP(AF.hi, DE.hi, op_cycles[opcode]);
    case 0xBB:
        CPU_8BIT_CP(AF.hi, DE.lo, op_cycles[opcode]);
    case 0xBC:
        CPU_8BIT_CP(AF.hi, HL.hi, op_cycles[opcode]);
    case 0xBD:
        CPU_8BIT_CP(AF.hi, HL.lo, op_cycles[opcode]);
    case 0xBE:
        uint8_t n = readByte(HL.value);
        CPU_8BIT_CP(AF.hi, n, op_cycles[opcode]);
        break;
    case 0xBF:
        CPU_8BIT_CP(AF.hi, AF.hi, op_cycles[opcode]);
        break;
    case 0xC0: // TODO: RET NZ
    {
        if (getZeroFlag() == false)
            CPU_RET();
    }
    break;
    case 0xC1:
        CPU_POP(BC, op_cycles[opcode]);
        break;
    case 0xC2: // TODO: JP NZ,nn
    {
        uint16_t nn = memory->readWord(pc);
        if (getZeroFlag() == 0)
            CPU_JP(nn);
    }
    break;
    case 0xC3: // TODO
    {
        uint16_t nn = memory->readWord(pc);
        CPU_JP(nn);
    }
    break;
    case 0xC4: // TODO: call NZ,nn
    {
        uint16_t nn = memory->readWord(pc);
        if (getZeroFlag() == 0)
            CPU_CALL(nn);
    }
    break;
    case 0xC5:
        CPU_PUSH(BC, op_cycles[opcode]);
        break;
    case 0xC6:
        CPU_8BIT_ADD(AF.hi, 0, op_cycles[opcode], true, false);
        break;
    case 0xC7: // TODO: RST 00H
        CPU_RST(0x00, op_cycles[opcode]);
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
        uint16_t nn = memory->readWord(pc);
        if (getZeroFlag() == 1)
            CPU_JP(nn);
    }
    break;
    case 0xCB: // TODO: prefix CB
        executeExtendedOpcode();
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
        uint16_t nn = memory->readWord(pc);
        CPU_CALL(nn);
    }
    break;
    case 0xCE: // ADC A, #
        CPU_8BIT_ADD(AF.hi, 0, op_cycles[opcode], true, true);
        break;
    case 0xCF: // RST 08H
        CPU_RST(0x08, op_cycles[opcode]);
        break;
    case 0xD0: // TODO: RET NC
        // CPU_RET();
        break;
    case 0xD1:
        CPU_POP(DE, op_cycles[opcode]);
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
        CPU_PUSH(DE, op_cycles[opcode]);
        break;
    case 0xD6:
        CPU_8BIT_SUB(AF.hi, 0, op_cycles[opcode], true, false);
        break;
    case 0xD7: // TODO: RST 10H
        CPU_RST(0x10, op_cycles[opcode]);
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
    case 0xDB: // TODO: None?
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
        CPU_SBC(AF.hi, op_cycles[opcode]);
        break;
    case 0xDF: // TODO: RST 18H
        CPU_RST(0x18, op_cycles[opcode]);
        break;
    case 0xE0:
    {
        uint8_t n = readByte(pc);
        pc++;
        writeByte((0xFF00 + n), AF.hi);
        // clockCycles += op_cycles[opcode];
    }
    break;
    case 0xE1:
        CPU_POP(HL, op_cycles[opcode]);
        break;
    case 0xE2: // TODO
    {
        writeByte((0xFF00 + BC.lo), AF.hi);
        // clockCycles += op_cycles[opcode];
    }
    break;
    // case 0xE3: // TODO: None?
    // case 0xE4: // TODO: also none
    case 0xE5:
        CPU_PUSH(HL, op_cycles[opcode]);
        break;
    case 0xE6: // TODO: Check
    {
        uint8_t op = readByte(pc);
        CPU_8BIT_AND(AF.hi, op, op_cycles[opcode], true);
    }
    break;
    case 0xE7: // TODO: RST 20H
        CPU_RST(0x20, op_cycles[opcode]);
        break;
    case 0xE8: // TODO: check
    {
        uint8_t n = readByte(pc++);
        CPU_16BIT_ADD(sp, n, op_cycles[opcode]);
    }
    break;
    case 0xE9:
        CPU_JP(HL.value);
        break;
    case 0xEA: // TODO
    {
        uint16_t nn = readWord(pc + 1);
        nn = nn << 8;
        nn |= readWord(pc);
        pc += 2;
        writeByte(nn, AF.hi);
        // clockCycles += op_cycles[opcode];
    }
    break;
    // case 0xEB: // TODO: None for EB, EC, ED
    // case 0xEC:
    // case 0xED:
    case 0xEE: // TODO: check
    {
        uint8_t op = readByte(pc);
        CPU_8BIT_XOR(AF.hi, op, op_cycles[opcode], true);
    }
    break;
    case 0xEF: // TODO: RST 28H
        CPU_RST(0x28, op_cycles[opcode]);
        break;
    case 0xF0:
    {
        uint8_t n = readByte(pc);
        pc++;
        CPU_REG_ROM_LD(AF.hi, (0xFF00 + n), op_cycles[opcode]);
    }
    break;
    case 0xF1:
        CPU_POP(AF, op_cycles[opcode]);
        break;
    case 0xF2: // TODO: check correctness
        CPU_REG_ROM_LD(AF.hi, (0xFF00 + BC.lo), op_cycles[opcode]);
        break;
    case 0xF3: // DI, aka disable interrupt
        // op_set_ime()
        CPU_DI();
        break;
    // case 0xF4: // TODO: None?
    case 0xF5:
        CPU_PUSH(AF, op_cycles[opcode]);
        break;
    case 0xF6:
    {
        uint8_t n = readByte(pc);
        pc++;
        CPU_8BIT_OR(AF.hi, n, op_cycles[opcode], true);
    }
    break;
    case 0xF7: // TODO: RST 30H
        CPU_RST(0x30, op_cycles[opcode]);
        break;
    case 0xF8: // TODO
        // CPU_16BIT_REG_LD(HL.value, sp+n, op_cycles[opcode]);
        break;
    case 0xF9: // TODO: check accuracy
        CPU_16BIT_REG_LD(sp, HL.value, op_cycles[opcode]);
        break;
    case 0xFA: // TODO
    {
        uint16_t nn = readWord(pc);
        pc += 2;
        uint8_t n = readByte(nn);
        AF.hi = n;
    }
    break;
    case 0xFB: // EI / Enable Interrupt
        // op_set_ime()
        CPU_EI();
        break;
    case 0xFE: // TODO: check
    {
        uint8_t op = readByte(pc);
        CPU_8BIT_CP(AF.hi, op, op_cycles[opcode]);
    }
    break;
    case 0xFF: // TODO: RST 38H
        CPU_RST(0x38, op_cycles[opcode]);
        break;
    case 0xFC: // illegal
    case 0xFD: // illegal
    default:
    {
        printf("Unsupported opcode: 0x%02x at 0x%04x\n\n\n", opcode, pc);
        exit(EXIT_FAILURE);
        pc++;
    }
    break;
    }
    return op_cycles[opcode];
}

bool CPU::checkInterrupts()
{
}

int CPU::handleInterrupts()
{
    // TODO
    return 0;
}
// TODO: finsih & check for semantic errors
int CPU::tick()
{
    while (!cpuStopped)
    {
        // 1. fetch instruction
        uint8_t opcode = fetch();
        // 2. decode & execute
        clockCycles -= execute(opcode);
        // 3. check & handle interrupts
        if (clockCycles <= 0)
        {
            clockCycles += handleInterrupts();
            // if (exit) break;
        }
    }
    return clockCycles;
}

void CPU::executeExtendedOpcode()
{
    uint8_t op = readByte(pc++);
    // pc++;
    switch (op)
    {
    // RLC n
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    // RL n
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    // RRC n
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    // RR n
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    // SLA n
    case 0x27:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    // SRA n
    case 0x2F:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
    // SRL n
    case 0x3F:
    case 0x38:
    case 0x39:
    case 0x3A:
    case 0x3B:
    case 0x3C:
    case 0x3D:
    case 0x3E:
    // BIT b,r
    case 0x47:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    // SET b,r
    case 0xC7:
    case 0xC0:
    case 0xC1:
    case 0xC2:
    case 0xC3:
    case 0xC4:
    case 0xC5:
    case 0xC6:
    // RES b,r
    case 0x87:
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x73:
    default:
        // TODO: unrecognized code
    }
}