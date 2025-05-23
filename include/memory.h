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
class MMU
{
private:
    uint8_t rom_bank_no; // always 1 or above, never 0
    uint8_t ram_bank_no; // ram bank currently in use
    uint8_t m_rom_banks[0x200000];
    uint8_t m_ram_banks[0x8000];
    uint8_t memory[0x10000]; // 65536 bytes
    uint8_t cartridge[0x100];
    bool MBC1, MBC2, MBC3;
    bool enable_ram;
    bool rom_banking;
    bool ime;

public:
    MMU();
    void loadROM(char *rom, size_t size);
    void loadBootROM();
    void unloadBootROM();
    // memory banking
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
    // void memory_dump();
    void getJoypadState(); // for 0xFF00
    //  read & write
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
};