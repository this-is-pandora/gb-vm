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
- [x] Implement functions for graphical calculations & data
- [x] Implement a window & functions to render graphics
- [ ] Keyboard input & key-mapping
- [x] Write the emulation code: emulation loop, rendering, etc.
- [ ] Write unit tests & general testing
- [ ] Rework the code for interrupt handling w/ stricter separation between memory handling routines and interrupt handling routines
- [ ] Rewrite some of the code to utilize the singleton pattern
- [x] Write CMake code to compile program

TODO: Other features & additions (if I have time)
- [ ] Make the emulator playable via the web browser (maybe using WebAssembly)
- [ ] Menu to select and load .gb file
- [ ] Implement MBC1
- [ ] Implement other MBCs
- [ ] Refactoring: optimize & clean up code
- [ ] Color
- [ ] Audio & sound
- [ ] Save & load game
- [x] Exit

How to run:
```
cmake -S . -B build
cmake --build build
./build/gbvm
```

Screenshots:
<img width="877" alt="booting" src="https://github.com/this-is-pandora/gb-vm/assets/32402885/647e84a5-e099-4a1c-ad04-dff6e9697864">

Useful resources:
https://gbdev.io/pandocs/

https://fms.komkon.org/EMUL8/HOWTO.html

https://hacktix.github.io/GBEDG/ppu/#the-concept-of-scanlines

https://github.com/AntonioND/giibiiadvance/blob/master/docs/other_docs/GBSOUND.txt

https://izik1.github.io/gbops/index.html