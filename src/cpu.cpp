#include "../include/cpu.h"

CPU::CPU() {
    AF.value = 0x01B0;
    BC.value = 0x0013;
    DE.value = 0x00D8;
    HL.value = 0x014D;
    clockCycles = 0;
    clockSpeed = 1024;
    isHalted = false;
    m_ROM[0xFF05] = 0x00;
    m_ROM[0xFF06] = 0x00;
    m_ROM[0xFF07] = 0x00;
    m_ROM[0xFF10] = 0x80;
    m_ROM[0xFF11] = 0xBF;
    m_ROM[0xFF12] = 0xF3;
    m_ROM[0xFF14] = 0xBF;
    m_ROM[0xFF16] = 0x3F;
    m_ROM[0xFF17] = 0x00;
    m_ROM[0xFF19] = 0xBF;
    m_ROM[0xFF1A] = 0x7F;
    m_ROM[0xFF1B] = 0xFF;
    m_ROM[0xFF1C] = 0x9F;
    m_ROM[0xFF1E] = 0xBF;
    m_ROM[0xFF20] = 0xFF;
    m_ROM[0xFF21] = 0x00;
    m_ROM[0xFF22] = 0x00;
    m_ROM[0xFF23] = 0xBF;
    m_ROM[0xFF24] = 0x77;
    m_ROM[0xFF25] = 0xF3;
    m_ROM[0xFF26] = 0xF1;
    m_ROM[0xFF40] = 0x91;
    m_ROM[0xFF42] = 0x00;
    m_ROM[0xFF43] = 0x00;
    m_ROM[0xFF45] = 0x00;
    m_ROM[0xFF47] = 0xFC;
    m_ROM[0xFF48] = 0xFF;
    m_ROM[0xFF49] = 0xFF;
    m_ROM[0xFF4A] = 0x00;
    m_ROM[0xFF4B] = 0x00;
    m_ROM[0xFFFF] = 0x00;
}

void CPU::CPU_NOP(uint8_t cycles) {
    pc++;
    clockCycles += cycles;
}

void CPU::writeByte(uint16_t addr, uint8_t data) {
    m_ROM[addr] = data;
}

void CPU::writeWord(uint16_t addr, uint16_t data) {
    m_ROM[addr] = data;
}

uint8_t CPU::read(uint16_t addr) {
    // read ROM bank/Cartridge ROM
    if (addr >= 0x4000 && addr <= 0x7FFF){
        //TODO
    }
    // read RAM bank/Cartridge RAM
    else if (addr >= 0xA000 && addr <= 0xBFFF) {
        //TODO
    }
    // read joypad state
    else if (addr == 0xFF00) {
        //TODO
    }
    else {
        //TODO
        return m_ROM[addr];
    }
}

uint8_t CPU::readByte(uint16_t addr) {
    uint8_t data = m_ROM[addr];
    return data;
}

uint16_t CPU::readWord(uint16_t addr) {
    uint16_t data = m_ROM[addr];
    return data;
}

uint8_t CPU::fetch() {
    return m_ROM[pc++];
}

//TODO fix
// register to register
void CPU::CPU_REG_LD(uint8_t& reg, uint8_t load, uint8_t cycles) {
    reg = load;
    clockCycles += cycles;
}

// memory to register
void CPU::CPU_REG_ROM_LD(uint8_t& reg, uint16_t addr, uint8_t cycles) {
    reg = m_ROM[addr];
    clockCycles += cycles;
}

// register to memory
void CPU::CPU_ROM_REG_LD(uint16_t addr, uint8_t reg, uint8_t cycles) {
    m_ROM[addr] = reg;
    clockCycles += cycles;
}

void CPU::CPU_8BIT_LD(uint8_t& reg, uint8_t cycles) {
    uint8_t value = m_ROM[pc];
    reg = value;
    clockCycles += cycles;
}

void CPU::CPU_8BIT_ADD(uint8_t& reg, uint8_t add, uint8_t cycles, bool useImmediate, bool addCarry) {
    //TODO
    reg += add;
    clockCycles += cycles;
}

void CPU::execute(uint8_t opcode) {
    switch(opcode) {
        case 0x00: //NOP
            clockCycles += op_cycles[opcode];
            pc++;
            break;
        case 0x01: // ld bc, imm
            CPU_16BIT_LD(BC.value, op_cycles[opcode]);
            break;
        case 0x02: // ld (bc), a
            CPU_ROM_REG_LD(BC.value, AF.hi, op_cycles[opcode]);
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
            CPU_8BIT_LD(BC.hi, op_cycles[opcode]);
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0A:
            CPU_REG_ROM_LD(AF.hi, BC.value, op_cycles[opcode]);
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
            CPU_8BIT_LD(BC.lo, op_cycles[opcode]);
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12: // LD (DE), A
            CPU_ROM_REG_LD(DE.value, AF.hi, op_cycles[opcode]);
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
            CPU_8BIT_LD(DE.hi, op_cycles[opcode]);
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1A:
            CPU_REG_ROM_LD(AF.hi, DE.value, op_cycles[opcode]);
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            CPU_8BIT_LD(DE.lo, op_cycles[opcode]);
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
            CPU_ROM_REG_LD(HL.value, AF.hi, op_cycles[opcode]);
            CPU_16BIT_INC(HL.value, 0);
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
            CPU_8BIT_LD(HL.hi, op_cycles[opcode]);
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2A:
            CPU_REG_ROM_LD(AF.hi, HL.value, op_cycles[opcode]);
            CPU_16BIT_INC(HL.value, 0);
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2E:
            CPU_8BIT_LD(HL.lo, op_cycles[opcode]);
        case 0x2F:
        case 0x30:
        case 0x31:
        case 0x32: // LD (HLD),A
            CPU_ROM_REG_LD(HL.value, AF.hi, op_cycles[opcode]);
            CPU_16BIT_DEC(HL.value, 0);
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36: //TODO
            //clockCycles += 12;
            //uint8_t n = readMemory(pc);
            //pc++;
            //writeByte(HL.value, n);
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3A: //LD A,(HLD)
            CPU_REG_LD(AF.hi, HL.value, op_cycles[opcode]);
            CPU_16BIT_DEC(HL.value, 0);
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E: //TODO
            //clockCycles += 8;
            //uint8_t n = readMemory(pc);
            // pc++;
            //AF.hi = n;
        case 0x3F:
        case 0x40:
            CPU_REG_LD(BC.hi, BC.hi, op_cycles[opcode]);
        case 0x41:
            CPU_REG_LD(BC.hi, BC.lo, op_cycles[opcode]);
        case 0x42:
            CPU_REG_LD(BC.hi, DE.hi, op_cycles[opcode]);
        case 0x43:
            CPU_REG_LD(BC.hi, DE.lo, op_cycles[opcode]);
        case 0x44:
            CPU_REG_LD(BC.hi, HL.hi, op_cycles[opcode]);
        case 0x45:
            CPU_REG_LD(BC.hi, HL.lo, op_cycles[opcode]);
        case 0x46:
            CPU_REG_ROM_LD(BC.hi, HL.value, op_cycles[opcode]);
        case 0x47:
        case 0x48:
            CPU_REG_LD(BC.lo, BC.hi, op_cycles[opcode]);
        case 0x49:
            CPU_REG_LD(BC.lo, BC.lo, op_cycles[opcode]);
        case 0x4A:
            CPU_REG_LD(BC.lo, DE.hi, op_cycles[opcode]);
        case 0x4B:
            CPU_REG_LD(BC.lo, DE.lo, op_cycles[opcode]);
        case 0x4C:
            CPU_REG_LD(BC.lo, HL.hi, op_cycles[opcode]);
        case 0x4D:
            CPU_REG_LD(BC.lo, HL.lo, op_cycles[opcode]);
        case 0x4E:
            CPU_REG_ROM_LD(BC.lo, HL.value, op_cycles[opcode]); // ld C, (HL)
        case 0x4F:
        case 0x50:
            CPU_REG_LD(DE.hi, BC.hi, op_cycles[opcode]);
        case 0x51:
            CPU_REG_LD(DE.hi, BC.lo, op_cycles[opcode]);
        case 0x52:
            CPU_REG_LD(DE.hi, DE.hi, op_cycles[opcode]);
        case 0x53:
            CPU_REG_LD(DE.hi, DE.lo, op_cycles[opcode]);
        case 0x54:
            CPU_REG_LD(DE.hi, HL.hi, op_cycles[opcode]);
        case 0x55:
            CPU_REG_LD(DE.hi, HL.lo, op_cycles[opcode]);
        case 0x56:
            CPU_REG_ROM_LD(DE.hi, HL.value, op_cycles[opcode]);
        case 0x57:
        case 0x58:
            CPU_REG_LD(DE.lo, BC.hi, op_cycles[opcode]);
        case 0x59:
            CPU_REG_LD(DE.lo, BC.lo, op_cycles[opcode]);
        case 0x5A:
            CPU_REG_LD(DE.lo, DE.hi, op_cycles[opcode]);
        case 0x5B:
            CPU_REG_LD(DE.lo, DE.lo, op_cycles[opcode]);
        case 0x5C:
            CPU_REG_LD(DE.lo, HL.hi, op_cycles[opcode]);
        case 0x5D:
            CPU_REG_LD(DE.lo, HL.lo, op_cycles[opcode]);
        case 0x5E:
            CPU_REG_ROM_LD(DE.lo, HL.value, op_cycles[opcode]);
        case 0x5F:
        case 0x60: //LD H,B
            CPU_REG_LD(HL.hi, BC.hi, op_cycles[opcode]);
        case 0x61: //LD H,C
            CPU_REG_LD(HL.hi, BC.lo, op_cycles[opcode]);
        case 0x62: //LD H,D
            CPU_REG_LD(HL.hi, DE.hi, op_cycles[opcode]);
        case 0x63: //LD H,E
            CPU_REG_LD(HL.hi, DE.lo, op_cycles[opcode]);
        case 0x64: //LD H,H
            CPU_REG_LD(HL.hi, HL.hi, op_cycles[opcode]);
        case 0x65:
            CPU_REG_LD(HL.hi, HL.lo, op_cycles[opcode]);
        case 0x66:
            CPU_REG_ROM_LD(HL.hi, HL.value, op_cycles[opcode]);
        case 0x67:
        case 0x68:
            CPU_REG_LD(HL.lo, BC.hi, op_cycles[opcode]);
        case 0x69:
            CPU_REG_LD(HL.lo, BC.lo, op_cycles[opcode]);
        case 0x6A:
            CPU_REG_LD(HL.lo, DE.hi, op_cycles[opcode]);
        case 0x6B:
            CPU_REG_LD(HL.lo, DE.lo, op_cycles[opcode]);
        case 0x6C:
            CPU_REG_LD(HL.lo, HL.hi, op_cycles[opcode]);
        case 0x6D:
            CPU_REG_LD(HL.lo, HL.lo, op_cycles[opcode]);
        case 0x6E:
            CPU_REG_ROM_LD(HL.lo, HL.value, op_cycles[opcode]);
        case 0x6F:
        case 0x70: //LD (HL),B
            CPU_ROM_REG_LD(HL.value, BC.hi, op_cycles[opcode]);
        case 0x71:
            CPU_ROM_REG_LD(HL.value, BC.lo, op_cycles[opcode]);
        case 0x72:
            CPU_ROM_REG_LD(HL.value, DE.hi, op_cycles[opcode]);
        case 0x73:
            CPU_ROM_REG_LD(HL.value, DE.lo, op_cycles[opcode]);
        case 0x74:
            CPU_ROM_REG_LD(HL.value, HL.hi, op_cycles[opcode]);
        case 0x75:
            CPU_ROM_REG_LD(HL.value, HL.lo, op_cycles[opcode]);
        case 0x76:
        case 0x77:
            CPU_ROM_REG_LD(HL.value, AF.hi, op_cycles[opcode]);
        case 0x78:
            CPU_REG_LD(AF.hi, BC.hi, op_cycles[opcode]);
        case 0x79:
            CPU_REG_LD(AF.hi, BC.lo, op_cycles[opcode]);
        case 0x7A:
            CPU_REG_LD(AF.hi, DE.hi, op_cycles[opcode]);
        case 0x7B:
            CPU_REG_LD(AF.hi, DE.lo, op_cycles[opcode]);
        case 0x7C:
            CPU_REG_LD(AF.hi, HL.hi, op_cycles[opcode]);
        case 0x7D:
            CPU_REG_LD(AF.hi, HL.lo, op_cycles[opcode]);
        case 0x7E:
            CPU_REG_ROM_LD(AF.hi, HL.value, op_cycles[opcode]);
        case 0x7F:
            CPU_REG_LD(AF.hi,AF.hi, op_cycles[opcode]);
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF:
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
        case 0xC0:
        case 0xC1:
            CPU_POP(BC.value, op_cycles[opcode]);
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
            CPU_PUSH(BC.value, op_cycles[opcode]);
        case 0xC6:
        case 0xC7:
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
        case 0xD0:
        case 0xD1:
            CPU_POP(DE.value, op_cycles[opcode]);
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
            CPU_PUSH(DE.value, op_cycles[opcode]);
        case 0xD6:
        case 0xD7:
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF:
        case 0xE0:
            //uint8_t n = readMemory(pc);
            //pc++;
            //writeByte(($FF00 + n), AF.hi, op_cycles[opcode]);
        case 0xE1:
            CPU_POP(HL.value, op_cycles[opcode]);
        case 0xE2: //TODO
            //writeByte((0xFF00 + BC.lo), AF.hi, op_cycles[opcode]);
        case 0xE3:
        case 0xE4:
        case 0xE5:
            CPU_PUSH(HL.value, op_cycles[opcode]);
        case 0xE6:
        case 0xE7:
        case 0xE8:
        case 0xE9:
        case 0xEA: //TODO
            //clockCycles += 16;
            //uint16_t nn = readWord();
            //pc += 2;
            //writeByte(nn, AF.hi);
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
        case 0xF0:
            //uint8_t n = readMemory(pc);
            //pc++;
            //CPU_REG_ROM_LD(AF.hi, (0xFF00 + n), op_cycles[opcode]);
        case 0xF1:
            CPU_POP(AF.value, op_cycles[opcode]);
        case 0xF2: //TODO
            //CPU_REG_ROM_LD(AF.hi, (0xFF00 + BC.lo), op_cycles[opcode]);
        case 0xF3: //EI
            //op_set_ime()
        case 0xF4:
        case 0xF5:
            CPU_PUSH(AF.value, op_cycles[opcode]);
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xFA: //TODO
            //uint16_t nn = readWord();
            //pc += 2;
            //uint8_t n = readMemory(nn);
            //AF.hi = n;
        case 0xFB: //DI
            // op_set_ime()
        case 0xFE:
        case 0xFF:
            //TODO
            break;
        case 0xFC: // illegal
        case 0xFD: // illegal
        default:
            printf("Unsupported opcode: 0x%02x at 0x%04x\n\n\n", opcode, pc);
            exit(EXIT_FAILURE);
            pc++;
            break;
    }
}

void CPU::run() {
    // cpu cycle:
    // 1. fetch instruction
    uint8_t opcode = fetch();
    // 2. decode & execute
    execute(opcode);
    // 3. check & handle interrupts
    handleInterrupts();
    // 4. repeat
}