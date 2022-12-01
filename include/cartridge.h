#ifndef CART_H
#define CART_H
#include <cstdint>

class Cartridge {

};

class ROMOnly : public Cartridge {
    public:
        uint8_t begin = 0x0000;
        uint8_t end = 0x7FFF;
        uint8_t size = 32000; //32kB

};
class MBC1 : public Cartridge {
    public:
        uint8_t ROM[];
};
class MBC2 : public Cartridge {};
class MBC3 : public Cartridge {};
class MBC5 : public Cartridge {};
class Rumble : public Cartridge {};
class HuC1 : public Cartridge {};

#endif