# about
An emulator application that's based on the original GameBoy with its Sharp SM83 processor. The implementation of the emulator itself is currently rather basic and still a WIP, but it should be able to run games that don't use MBC's, e.g. Tetris.

## technical specifications
The specifications of the original GameBoy are as follows: 

<table>
    <thead>
        <tr>
            <th>spec</th>
            <th>value</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Resolution</td>
            <td>160(w) x 144(h) pixels</td>
        </tr>
        <tr>
            <td>Frame rate</td>
            <td>≈59.7Hz</td>
        </tr>
        <tr>
            <td>Color support</td>
            <td>0x0, 0x1, 0x2, 0x3</td>
        </tr>
        <tr>
            <td>SoC</td>
            <td>Nintendo DMG-CPU</td>
        </tr>
        <tr>
            <td>WRAM</td>
            <td>8KB</td>
        </tr>
        <tr>
            <td>VRAM</td>
            <td>8KB</td>
        </tr>
        <tr>
            <td>External</td>
            <td>Up to 1MB ROM and 128KB RAM</td>
        </tr>
    </tbody>
</table>

## usage
This app can be run via the terminal like so:
```
./gbvm Tetris.gb
```
Or you can run the executable and load in a ROM file from memory.

The ROM files aren't packaged with this application so you'll have to download or buy the ROM files yourself from somewhere.

## building
Download the repo to your computer, then run:
```
cmake -S . -B build
cmake --build build
./build/gbvm
```
Be sure to drop the SDL2.dll into the /build/ directory or else the executable won't run.

## screenshots
<img width="877" alt="booting" src="https://github.com/this-is-pandora/gb-vm/assets/32402885/647e84a5-e099-4a1c-ad04-dff6e9697864">

## resources:
https://gbdev.io/pandocs/

https://fms.komkon.org/EMUL8/HOWTO.html

https://hacktix.github.io/GBEDG/ppu/#the-concept-of-scanlines

https://github.com/AntonioND/giibiiadvance/blob/master/docs/other_docs/GBSOUND.txt

https://izik1.github.io/gbops/index.html