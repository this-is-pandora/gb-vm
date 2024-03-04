// #include SDL2
#include "../include/gameboy.h"

using namespace std;

const int fps = 60;

// takes in a rom file as an argument
// command line example: gb tetris.rom
// max game size is 0x200000
int main(int argc, char **argv[])
{
    bool exit = false;
    GameBoy *gb = new GameBoy();
    // load game into gb
    // initialize emulator, game window, etc.
    // run game loop
    while (!exit)
    {
        // handle SDL poll events
        gb->emulate(); // play game
    }
    delete gb;
    return 1;
}