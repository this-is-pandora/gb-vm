#pragma once
#include <stdint.h>
#include <cstring>
/*
0x0000-0x3FFF: Permanent ROM Bank (16KB)
0x4000-0x7FFF: switchable ROM banks area (16KB)
0x8000-0x9FFF: VRAM (8KB)
0xA000-0xBFFF: switchable RAM banks area (8KB)
0xC000-0xCFFF: WRAM bank 0 (4KB)
0xD000-0xDFFF: WRAM bank 1 (4KB)
0xFE00-0xFE9F: Sprite Attribute Table (OAM)
0xFEA0-0xFEFF: Unusable
0xFF00-0xFF7F: I/O ports
0xFF80-0xFFFE: HRAM
0xFFFF: IER
*/

// TODO: implement a Memory class
class MMU
{
private:
    uint8_t rom_bank_no; // always 1 or above, never 0
    uint8_t ram_bank_no;
    // in the cartridge
    // each ROM bank is 16KB, so 125 ROM banks * 16KB = 2,000,000
    uint8_t m_rom_banks[0x200000];
    // 4 RAM banks * 0x2000 bytes each = 0x8000 total
    uint8_t m_ram_banks[0x8000];
    uint8_t bootloader[0x100]; // stores booting code DMG_bin
    uint8_t m_ROM[0x10000];    // 65536

    bool MBC1, MBC2, MBC3;
    bool enable_ram;
    bool rom_banking;
    bool ime;

public:
    MMU();
    void loadROM(char *rom);
    // banking stuff
    void checkMBC();
    void enableRAMBank(uint16_t addr, uint8_t value);
    void changeROMBank(uint8_t value);
    void changeLowROMBank(uint8_t value);
    void changeHighROMBank(uint8_t value);
    void changeRAMBank(uint8_t value);
    void switchModes(uint8_t value);
    void handleBanking(uint16_t addr, uint8_t value);
    // for interrupt enabling/disabling
    void enableInterrupts(bool value);
    bool interruptsEnabled();
    void memory_dump();
    // read & write
    void writeByte(uint16_t addr, uint8_t value);
    void writeWord(uint16_t addr, uint16_t value);
    uint8_t readByte(uint16_t addr);
    uint16_t readWord(uint16_t addr);
    void incAddr(uint16_t addr);
    // increment divider register
    void incDIV();
    // direct memory access
    void DMATransfer(uint8_t value);
    // stack methods
    void push(uint16_t value, uint16_t &sp);
    uint16_t pop(uint16_t &sp);

    MMU()
    {
        rom_bank_no = 1;
        ram_bank_no = 0;
        enable_ram = false;
        memset(&m_ram_banks, 0, sizeof(m_ram_banks));

        m_ROM[0xFF05] = 0x00;
        m_ROM[0xFF06] = 0x00;
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
};