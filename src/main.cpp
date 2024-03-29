// #include SDL2
#include "../include/gameboy.h"

using namespace std;

// takes in a rom file as an argument
// command line example: gb tetris.rom
// max game size is 0x200000
int main(int argc, char **argv[])
{
    SDL_Event event;
    bool exit = false;
    GameBoy *gb = new GameBoy();
    // TODO: init gameboy elements
    // load game into gb
    // initialize emulator, game window, etc.
    // run game loop
    while (!exit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                exit = true;
        }
        gb->emulate(); // play game
    }
    delete gb;
    return 0;
}