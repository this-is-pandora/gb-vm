A gameboy emulator written in C++
No sound or color is implemented (yet).
Will mainly only concern myself with games w/o an MBC, e.g. Tetris

TODO: Most important
- [ ] Implement the CPU instruction set, i.e. the opcodes
- [x] Implement the ROM, RAM, & other memory-implements
- [x] Implement DMA transfer
- [x] Ability to load a ROM or .gb file into memory
- [ ] Implement interrupts & interrupt handling
- [ ] Implement CPU clock & other timing elements
- [ ] Implement functions for graphical calculations & data
- [ ] Implement a window & functions to render graphics
- [ ] Keyboard input & key-mapping
- [ ] Write the emulation code: emulation loop, rendering, etc.
- [ ] Write unit tests & general testing

TODO: Other features & additions (if I have time)
- [ ] Menu to select and load .gb file
- [ ] Implement MBC1, MBC2
- [ ] Implement other MBCs
- [ ] Optimize & clean up code
- [ ] Color
- [ ] Audio & sound
- [ ] Save game
- [ ] Exit

Useful sources: