#pragma once
#include <stdint.h>
#include <memory>
// total RAM size: 0xFFFF

// work RAM: 0xC000 - 0xDFFF
// external RAM: 0xA000 - 0xBFFF 
// echo RAM: E000 - FDFF
// vram: 0x8000 - 9FFF (8kB)
// IO: 0xFF00 -
// hram: 0xFF80 - 0xFFFE

// addresses of memory parts

//32kB cartridge (ROM bank + switchable ROM bank) i.e. where the game is loaded
#define ROM_BANK 0x0000 // to (16kB ROM BANK #0)
#define SWITCH_ROM_BANK 0x4000 // to (16kB switchable ROM bank)

#define VRAM 0x8000 // to 0x9FFF
#define SWITCH_RAM_BANK 0xA000 // to 
#define MRAM 0xC000 // to 0xDFFF (work RAM)
#define ECHO_RAM 0xE000 // to 0x9FFF
#define OAM 0xFE00 // to (Sprite Atrib Memory)
#define EMPTY_UNUSUBLE_IO_1 0xFEA0 // to
#define IO_PORTS 0xFF00 // to 
#define EMPTY_UNUSABLE_IO_2 0xFF4C // to
#define HRAM 0xFF80 //to 0xFFFE (HRAM)
#define IER 0xFFFF //to (Interrupt Enable Register)

//TODO reserved memory locations

//TODO: implement a Memory class
class Memory {
    private:        
        //TODO: assign these varaibles their proper types
        int mbc = 0;
        int mbc_mode = 0;
        int rom_bank = 1;
        int rom_banks = 0;
        int ram_exists = 0;
        int ram_enable = 0;
        int ram_bank = 0;
        int ram_banks = 0;

        uint8_t m_ROM[0x10000];
    public:
        //read & write
        void writeByte(uint16_t addr, uint8_t value);
        void writeWord(uint16_t addr, uint16_t value);
        uint8_t readByte(uint16_t addr);
        uint16_t readWord(uint16_t addr);
        // stack methods
        void push();
        void pop();
        
        Memory() {
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