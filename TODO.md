## TODO: core features
- [x] Implement the CPU instruction set, i.e. the opcodes
- [x] Implement the ROM, RAM, & other memory-implements
- [x] Implement DMA transfer
- [x] Ability to load a ROM or .gb file into memory
- [x] Implement interrupts & interrupt handling
- [x] Implement CPU clock & other timing elements
- [x] Implement functions for graphical calculations & data
- [x] Implement a window & functions to render graphics
- [ ] Keyboard input & key-mapping
- [x] Write the emulation code: emulation loop, rendering, etc.
- [ ] Write unit tests & general testing
- [ ] Rework the code for interrupt handling w/ stricter separation between memory handling routines and interrupt handling routines
- [x] Write CMake code to compile program

## TODO: misc. features (if I have time)
- [ ] Make the emulator playable via the web browser (maybe using WebAssembly?)
- [ ] Menu to select and load .gb file
- [ ] Implement MBC1
- [ ] Implement other MBCs
- [ ] Refactoring: optimize & clean up code
- [ ] Color
- [ ] Audio & sound
- [ ] Save & load game
- [x] Exit