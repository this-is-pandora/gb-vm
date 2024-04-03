// #include SDL2
#define SDL_MAIN_HANDLED
#include "../include/gameboy.h"
#include <iostream>
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
    SDL_Event event;
    bool exit = false;
    std::cout << "creating GameBoy instance.." << std::endl;
    GameBoy *gb = new GameBoy();
    std::cout << "initializing GameBoy.." << std::endl;
    gb->initGameBoy();
    std::cout << "Finished initialization.." << std::endl;
    /*
    while (!exit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                exit = true;
        }
        gb->emulate(); // play game
    } */
    std::cout << "Deleting.." << std::endl;
    delete gb;
    return 0;
}