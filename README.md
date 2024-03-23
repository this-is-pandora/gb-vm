A gameboy emulator written in C++
No sound or color is implemented (yet).
Will mainly only concern myself with games w/o an MBC, e.g. Tetris

TODO: Most important
- [x] Implement the CPU instruction set, i.e. the opcodes
- [x] Implement the ROM, RAM, & other memory-implements
- [x] Implement DMA transfer
- [x] Ability to load a ROM or .gb file into memory
- [x] Implement interrupts & interrupt handling
- [x] Implement CPU clock & other timing elements
- [ ] Implement functions for graphical calculations & data
- [ ] Implement a window & functions to render graphics
- [ ] Keyboard input & key-mapping
- [ ] Write the emulation code: emulation loop, rendering, etc.
- [ ] Write unit tests & general testing
- [ ] Write CMake code to compile program

TODO: Other features & additions (if I have time)
- [ ] Make the emulator playable via the web browser (maybe using WebAssembly)
- [ ] Menu to select and load .gb file
- [ ] Implement MBC1
- [ ] Implement other MBCs
- [ ] Refactoring: optimize & clean up code
- [ ] Color
- [ ] Audio & sound
- [ ] Save & load game
- [ ] Exit

Useful resources:
https://gbdev.io/pandocs/
https://fms.komkon.org/EMUL8/HOWTO.html
https://hacktix.github.io/GBEDG/ppu/#the-concept-of-scanlines
https://github.com/AntonioND/giibiiadvance/blob/master/docs/other_docs/GBSOUND.txt
https://izik1.github.io/gbops/index.html
