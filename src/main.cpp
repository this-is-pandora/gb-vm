// #include SDL2
#define SDL_MAIN_HANDLED
#include "../SDL2/include/SDL2/SDL.h"
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

int main(int argc, char *argv[])
{
    GameBoy *gb = new GameBoy();
    SDL_Event event;
    bool exit = false;
    gb->initialize();
    while (!exit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                exit = true;
            else
                gb->handleInput(event);
        }

        gb->emulate();
    }
    delete gb;
    return 0;
}