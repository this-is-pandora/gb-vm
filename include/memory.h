#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
//TODO: implement a Memory class
class Memory {
    public:
        uint8_t size;
        Memory(uint8_t size) {
            size = size;
        }
        void copy(uint16_t src, uint16_t dst, size_t len);
};

Memory sram = Memory(0x2000); // 8kB
Memory vram = Memory(0x2000);
#endif