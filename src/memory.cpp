#include "../include/memory.h"
#include <fstream>
#include <iostream>

using namespace std;
// total RAM size: 0xFFFF

// work RAM: 0xC000 - 0xDFFF
// external RAM: 0xA000 - 0xBFFF
// echo RAM: E000 - FDFF
// vram: 0x8000 - 9FFF (8kB)
// IO: 0xFF00 -
// hram: 0xFF80 - 0xFFFE
MMU::MMU()
{
    rom_bank_no = 1;
    ram_bank_no = 0;
    enable_ram = false;
    memset(&m_ram_banks, 0, sizeof(m_ram_banks));
    memset(&memory, 0, sizeof(memory));
    // booting = true;
    memory[0xFF05] = 0x00;
    memory[0xFF06] = 0x00;
    memory[0xFF05] = 0x00;
    memory[0xFF06] = 0x00;
    memory[0xFF07] = 0x00;
    memory[0xFF10] = 0x80;
    memory[0xFF11] = 0xBF;
    memory[0xFF12] = 0xF3;
    memory[0xFF14] = 0xBF;
    memory[0xFF16] = 0x3F;
    memory[0xFF17] = 0x00;
    memory[0xFF19] = 0xBF;
    memory[0xFF1A] = 0x7F;
    memory[0xFF1B] = 0xFF;
    memory[0xFF1C] = 0x9F;
    memory[0xFF1E] = 0xBF;
    memory[0xFF20] = 0xFF;
    memory[0xFF21] = 0x00;
    memory[0xFF22] = 0x00;
    memory[0xFF23] = 0xBF;
    memory[0xFF24] = 0x77;
    memory[0xFF25] = 0xF3;
    memory[0xFF26] = 0xF1;
    memory[0xFF40] = 0x91;
    memory[0xFF42] = 0x00;
    memory[0xFF43] = 0x00;
    memory[0xFF45] = 0x00;
    memory[0xFF47] = 0xFC;
    memory[0xFF48] = 0xFF;
    memory[0xFF49] = 0xFF;
    memory[0xFF4A] = 0x00;
    memory[0xFF4B] = 0x00;
    memory[0xFFFF] = 0x00;
}
// TODO: develop further. fix.
void MMU::loadROM(char *rom, size_t size)
{
    // 1. open the rom file
    // 2. read file into the ROM (should take up the first 32kB or so)
    // 3. close file
    /*memset(m_rom_banks, 0, sizeof(m_rom_banks));
    FILE *file = fopen(rom, "rb");
    fread(m_rom_banks, 1, 0x200000, file);
    fclose(file);*/
    FILE *file = fopen(rom, "rb");
    fread(memory, 1, size, file);
    fclose(file);
}
void MMU::loadBootROM()
{
    // std::cout << "loading boot ROM..\n";
    loadROM("../bin/dmg_boot.bin", 0x100);
    // FILE *file = fopen("../bin/dmg_boot.bin", "rb");
    // fread(bootloader, 1, 0x100, file);
    // fclose(file);
}

void MMU::unloadBootROM()
{
    // if the BANK register (0xFF50) is set to 1
    // unmap the boot rom
    /*
    if (memory[0xFF50] == 1)
    {
        for (int i = 0; i < 256; i++)
        {
        }
    }*/
    // booting = false;
    exit(EXIT_SUCCESS); // just quit
}

// MBC1 has 125 banks
// MBC2 has 16 banks
void MMU::checkMBC()
{
    switch (memory[0x147])
    {
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
uint8_t MMU::readByte(uint16_t addr)
{
    // below 0x4000 is ROM bank 0.
    // 0x4000 - 0x7FFF may contain any bank other than bank 0
    /*
    if (booting)
    {
        if (addr < 0x0100)
        {
            return bootloader[addr];
        }
        else
        {
            return memory[addr];
        }
    } */
    if (addr >= 0x4000 && addr <= 0x7FFF)
    {
        // ROM Bank 1..NN, in cartridge
        uint16_t n_addr = addr - 0x4000;
        return m_rom_banks[n_addr + (rom_bank_no * 0x4000)];
    }
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        // RAM bank AKA external RAM, i.e. in cartridge
        uint16_t n_addr = addr - 0xA000;
        return m_ram_banks[n_addr + (ram_bank_no * 0x2000)];
    }

    else if (addr == 0xFF00)
    {
        // TODO: read IO or joypad state
    }
    else
    {
        return memory[addr];
    }
}

uint16_t MMU::readWord(uint16_t addr)
{
    uint16_t value = memory[addr + 1];
    value = value << 8;
    value |= memory[addr];
    return value;
}

void MMU::enableRAMBank(uint16_t addr, uint8_t value)
{
    if (MBC2)
    {
        // return if bit 4 != 0
    }
    // get the lower nibble of value
    uint8_t test = value & 0xf;
    if (test == 0xA)
    {
        enable_ram = true;
    }
    else if (test == 0x0)
    {
        enable_ram = false;
    }
    else
    {
        // TODO: crash?
    }
}

void MMU::changeROMBank(uint8_t value)
{
    if (MBC2)
    {
        rom_bank_no = value & 0xf;
        if (rom_bank_no == 0)
            rom_bank_no++;
        return;
    }
    // for MBC1
    // lower 5 bits of value
    uint8_t lo = value & 31;
    rom_bank_no &= 224; // turn off lower 5 bits
    rom_bank_no |= lo;
    if (rom_bank_no == 0)
        rom_bank_no++;
}

void MMU::changeLowROMBank(uint8_t value)
{
    if (MBC2)
    {
        rom_bank_no = value & 0xf;
        if (rom_bank_no == 0)
            rom_bank_no++;
        return;
    }
    // for MBC1
    // lower 5 bits of value
    uint8_t lo = value & 31;
    rom_bank_no &= 224; // turn off lower 5
    rom_bank_no |= lo;
    if (rom_bank_no == 0)
        rom_bank_no++;
}

void MMU::changeHighROMBank(uint8_t value)
{
    rom_bank_no &= 31;
    value &= 224;
    rom_bank_no |= value;
    if (rom_bank_no == 0)
        rom_bank_no++;
}

void MMU::changeRAMBank(uint8_t value)
{
    ram_bank_no = value & 0x3;
}

// switch between RAM or ROM mode
void MMU::switchModes(uint8_t value)
{
    uint8_t data = value & 0x1;
    rom_banking = (data == 0) ? true : false;
    if (rom_banking)
        ram_bank_no = 0;
}

void MMU::handleBanking(uint16_t addr, uint8_t value)
{
    if (addr < 0x2000)
    {
        if (MBC1 || MBC2)
        {
            // enable RAM
            enableRAMBank(addr, value);
        }
    }
    else if (addr >= 0x2000 && addr < 0x4000)
    {
        // ROM bank change only
        if (MBC1 || MBC2)
        {
            // ROM bank change
            changeLowROMBank(value);
        }
    }
    else if (addr >= 0x4000 && addr < 0x6000)
    {
        // ROM bank or RAM bank change
        if (MBC1)
        {
            if (rom_banking)
            {
                // change hi rom bank
                changeHighROMBank(value);
            }
            else
            {
                // RAM bank change
                changeRAMBank(value);
            }
        }
    }
    else
    {
        // 0x6000 - 0x8000
        // decides whether we are doing ROM banking or RAM banking
        if (MBC1)
        {
            // switch between ROM or RAM mode
            switchModes(value);
        }
    }
}

// write methods
void MMU::writeByte(uint16_t addr, uint8_t value)
{
    if (addr < 0x8000)
    {
        // don't write anything to read-only memory
        // cout << "in read-only memory.." << endl;
        handleBanking(addr, value);
    }
    else if (addr >= 0xA000 && addr < 0xC000)
    {
        // cout << "enable ram" << endl;
        if (enable_ram)
        {
            uint16_t n_addr = addr - 0xA000;
            m_ram_banks[n_addr + (ram_bank_no * 0x2000)] = value;
        }
    }
    else if (addr >= 0xE000 && addr < 0xFE00)
    {
        // writing to ECHO RAM will also write to RAM
        // RAM written to: 0xC000 - 0xDE00
        // cout << "echo RAM" << endl;
        memory[addr] = value;
        writeByte(addr - 0x2000, value);
    }
    else if (addr >= 0xFEA0 && addr < 0xFEFF)
    {
        // cout << "restricted area" << endl;
        //  restricted area
    }
    else if (addr == 0xFF50)
    {
        // unload boot rom
        // cout << "unloading boot rom" << endl;
        unloadBootROM();
    }
    // divider register
    else if ((addr == 0xFF04) || (addr == 0xFF44))
    {
        // cout << "divider reg" << endl;
        memory[addr] = 0;
    }
    else if (addr == 0xFF46)
    {
        // cout << "direct memory access" << endl;
        DMATransfer(value);
    }
    else
    {
        // cout << "writing to memory.." << endl;
        memory[addr] = value;
    }
}

void MMU::writeWord(uint16_t addr, uint16_t value)
{
    memory[addr] = value;
}

void MMU::incAddr(uint16_t addr)
{
    memory[addr] += 1;
}

// TODO: Fix
void MMU::incDIV()
{
    uint8_t div_value = readByte(0xFF04);
    if (div_value == 0xFF)
    {
        memory[0xFF04] = 0;
    }
    else
    {
        memory[0xFF04]++;
    }
}

void MMU::DMATransfer(uint8_t src_dst)
{
    uint16_t addr = src_dst << 8; // get the src value
    uint8_t value;
    // start from OAM to end of OAM
    for (int i = 0; i < 0xA0; i++)
    {
        value = readByte(addr + i);
        writeByte(0xFE00 + i, value);
    }
}
// stack methods
void MMU::push(uint16_t value, uint16_t &sp)
{
    uint8_t hi = value >> 8;
    uint8_t lo = value & 0xFF;
    sp--;
    writeByte(sp, hi);
    sp--;
    writeByte(sp, lo);
}

uint16_t MMU::pop(uint16_t &sp)
{
    uint16_t value;
    value = readByte(sp + 1) << 8;
    value |= readByte(sp);
    sp += 2;
    return value;
}

void MMU::enableInterrupts(bool value)
{
    ime = value;
}

bool MMU::interruptsEnabled()
{
    return ime;
}