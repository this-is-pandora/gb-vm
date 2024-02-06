#pragma once
#include <stdint.h>
#include <stack>
#include <memory>
#include <cstring>
#include <string>
// total RAM size: 0xFFFF

// work RAM: 0xC000 - 0xDFFF
// external RAM: 0xA000 - 0xBFFF 
// echo RAM: E000 - FDFF
// vram: 0x8000 - 9FFF (8kB)
// IO: 0xFF00 -
// hram: 0xFF80 - 0xFFFE

enum MemorySegments {
    CARTRIDGE_ROM,
    VRAM,
    CARTRIDGE_RAM,
    WRAM_0,
    WRAM_N,
    OAM,
    IO_HRAM_IE,
    NO_SEGMENT
};

//TODO: implement a Memory class
class MMU {
    private:
        uint8_t rom_bank_no; // always 1 or above, never 0
        uint8_t ram_bank_no;
        // in the cartridge
        uint8_t m_rom_banks[0x200000];
        // 4 RAM banks * 0x2000 bytes each = 0x8000 total
        uint8_t m_ram_banks[0x8000];
        uint8_t m_ROM[0x10000];

        bool MBC1, MBC2, MBC3;
        bool enable_ram;
        bool rom_banking;
        bool ime;

    public:
        void loadROM(char* rom);
        void checkMBC();

        void enableRAMBank(uint16_t addr, uint8_t value);
        void changeROMBank(uint8_t value);
        void changeLowROMBank(uint8_t value);
        void changeHighROMBank(uint8_t value);
        void changeRAMBank(uint8_t value);
        void switchModes(uint8_t value);
        void handleBanking(uint16_t addr, uint8_t value);

        //read & write
        void writeByte(uint16_t addr, uint8_t value);
        void writeWord(uint16_t addr, uint16_t value);
        uint8_t readByte(uint16_t addr);
        uint16_t readWord(uint16_t addr);
        // increment divider register
        void incDIV();
        //direct memory access
        void DMATransfer(uint8_t value);
        // stack methods
        void push(uint16_t value, uint16_t& sp);
        uint16_t pop(uint16_t& sp);
        
        MMU() {
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