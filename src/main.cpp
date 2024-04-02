// #include SDL2
#include "../include/gameboy.h"

/*
 * takes in a rom file as an argument
 * command line example: gbvm tetris.rom
 * max game size is 0x200000

 * initialize emulator elements: cpu, gpu, window, etc.
 * load boot rom
 * load game rom
 * run game loop until exit
*/
int main(int argc, char **argv[])
{
    SDL_Event event;
    bool exit = false;
    GameBoy *gb = new GameBoy();
    gb->initGameBoy();
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