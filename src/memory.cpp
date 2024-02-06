#include "./include/memory.h"
#include <fstream>
using namespace std;
// total RAM size: 0xFFFF

// work RAM: 0xC000 - 0xDFFF
// external RAM: 0xA000 - 0xBFFF
// echo RAM: E000 - FDFF
// vram: 0x8000 - 9FFF (8kB)
// IO: 0xFF00 -
// hram: 0xFF80 - 0xFFFE

void MMU::loadROM(char* rom) {
    // 1. open the rom file
    // 2. read file into the ROM (should take up the first 32kB or so)
    // 3. close file
    memset(m_rom_banks, 0, sizeof(m_rom_banks));
    FILE *file = fopen(rom, "rb");
    fread(m_rom_banks, 1, 0x200000, file);
    fclose(file);
}

//MBC1 has 125 banks
//MBC2 has 16 banks
void MMU::checkMBC() {
    switch(m_ROM[0x147]) {
        case 1:
            MBC1 = true;
            break;
        case 2:
            MBC1 = true;
            break;
        case 3:
            MBC1 = true;
            break;
        case 5:
            MBC2 = true;
            break;
        case 6:
            MBC2 = true;
            break;
        default:
            break;
    }
}

// read methods
uint8_t MMU::readByte(uint16_t addr) {
    // below 0x4000 is ROM bank 0.
    // 0x4000 - 0x7FFF may contain any bank other than bank 0
    if (addr >= 0x4000 && addr <= 0x7FFF) {
        // ROM Bank 1..NN, in cartridge
        uint16_t n_addr = addr - 0x4000;
        return m_rom_banks[n_addr + (rom_bank_no * 0x4000)];
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        // RAM bank AKA external RAM, i.e. in cartridge
        uint16_t n_addr = addr - 0xA000;
        return m_ram_banks[n_addr + (ram_bank_no * 0x2000)];
    } else if (addr == 0xFF00) {
        // read IO or joypad state
    } else {
        return m_ROM[addr];
    }
}

uint16_t MMU::readWord(uint16_t addr) {
    uint16_t value = m_ROM[addr+1];
    value = value << 8;
    value |= m_ROM[addr];
    return value;
}

void MMU::enableRAMBank(uint16_t addr, uint8_t value) {
    if (MBC2) {
        // return if bit 4 != 0
    }
    // get the lower nibble of value
    uint8_t test = value & 0xf;
    if (test == 0xA) {
        enable_ram = true;
    }
    else if (test == 0x0) {
        enable_ram = false;
    }
    else {
        //TODO: crash?
    }
}

void MMU::changeROMBank(uint8_t value) {
    if (MBC2) {
        rom_bank_no = value & 0xf;
        if (rom_bank_no == 0) rom_bank_no++;
        return;
    }
    // for MBC1
    // lower 5 bits of value
    uint8_t lo = value & 31;
    rom_bank_no &= 224; // turn off lower 5 bits
    rom_bank_no |= lo;
    if (rom_bank_no == 0) rom_bank_no++;
}

void MMU::changeLowROMBank(uint8_t value) {
    if (MBC2) {
        rom_bank_no = value & 0xf;
        if (rom_bank_no == 0) rom_bank_no++;
        return;
    }
    // for MBC1
    // lower 5 bits of value
    uint8_t lo = value & 31;
    rom_bank_no &= 224; // turn off lower 5
    rom_bank_no |= lo;
    if (rom_bank_no == 0) rom_bank_no++;
}

void MMU::changeHighROMBank(uint8_t value) {
    rom_bank_no &= 31;
    value &= 224;
    rom_bank_no |= value;
    if (rom_bank_no == 0) rom_bank_no++;
}

void MMU::changeRAMBank(uint8_t value) {
    ram_bank_no = value & 0x3;
}

// switch between RAM or ROM mode
void MMU::switchModes(uint8_t value) {
    uint8_t data = value & 0x1;
    rom_banking = (data == 0) ? true : false;
    if (rom_banking) ram_bank_no = 0;
}

void MMU::handleBanking(uint16_t addr, uint8_t value) {
    if (addr < 0x2000) {
        if (MBC1 || MBC2) {
            // enable RAM
            enableRAMBank(addr, value);
        }
    }
    else if (addr >= 0x2000 && addr < 0x4000) {
        // ROM bank change only
        if (MBC1 || MBC2) {
            // ROM bank change
            changeLowROMBank(value);
        }
    }
    else if (addr >= 0x4000 && addr < 0x6000) {
        // ROM bank or RAM bank change
        if (MBC1) {
            if (rom_banking) {
                // change hi rom bank
                changeHighROMBank(value);
            }
            else {
                // RAM bank change
                changeRAMBank(value);
            }
        }
    }
    else {
        // 0x6000 - 0x8000
        // decides whether we are doing ROM banking or RAM banking
        if (MBC1) {
            // switch between ROM or RAM mode
            switchModes(value);
        }
    }
}

// write methods
void MMU::writeByte(uint16_t addr, uint8_t value) {
    if (addr < 0x8000) {
        // don't write anything to read-only memory
        handleBanking(addr, value);
    }
    else if (addr >= 0xA000 && addr < 0xC000) {
        if (enable_ram) {
            uint16_t n_addr = addr - 0xA000;
            m_ram_banks[n_addr + (ram_bank_no * 0x2000)] = value;
        }
    }
    else if (addr >= 0xE000 && addr < 0xFE00) {
        // writing to ECHO RAM will also write to RAM
        // RAM written to: 0xC000 - 0xDE00
        m_ROM[addr] = value;
        writeByte(addr - 0x2000, value);
    }
    else if (addr >= 0xFEA0 && addr < 0xFEFF) {
        // restricted area
    }
    /*
    else if (addr == TIMA) {
        uint8_t current_freq = readClockFrequency();
        m_ROM[TIMA] = value;
        uint8_t n_freq = readClockFrequency();
        if (current_freq != n_freq) {
            setClockFrequency();
        }
    }
    */
   // divider register
   else if ((addr == 0xFF04) || (addr == 0xFF44)) {
        m_ROM[addr] = 0;
   }
   else if (addr == 0xFF46) {
        DMATransfer(value);
   }
    else {
        m_ROM[addr] = value;
    }
}

void MMU::writeWord(uint16_t addr, uint16_t value) {
    m_ROM[addr] = value;
}

//TODO: Fix
void MMU::incDIV() {
    uint8_t div_value = readByte(0xFF04);
    if (div_value == 0xFF) {
        m_ROM[0xFF04] = 0;
    } else {
        m_ROM[0xFF04]++;
    }
}

void MMU::DMATransfer(uint8_t src_dst) {
    uint16_t addr = src_dst << 8; // get the src
    uint8_t value;
    // start from OAM to end of OAM
    for (int i = 0; i < 0xA0; i++) {
        value = readByte(addr+i);
        writeByte(0xFE00+i, value);
    }
}
// stack methods
void MMU::push(uint16_t value, uint16_t& sp) {
    uint8_t hi = value >> 8;
    uint8_t lo = value & 0xFF;
    sp--;
    writeByte(sp, hi);
    sp--;
    writeByte(sp, lo);
}

uint16_t MMU::pop(uint16_t& sp) {
    uint16_t value;
    value = readByte(sp+1) << 8;
    value |= readByte(sp);
    sp += 2;
    return value;
}
