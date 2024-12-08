#include "../include/gameboy.h"
#include <iostream>

GameBoy::GameBoy()
{
    cycles = 0;
    total_cycles = 0;
    unpaused = true;
}

GameBoy::~GameBoy()
{
    delete cpu;
    delete gpu;
    // delete mmu;
}
void GameBoy::loadMemory(char *rom)
{
    mmu->loadROM(rom, 0x8000);
    mmu->loadBootROM();
}
void GameBoy::initialize()
{
    // mmu->loadBootROM();
    mmu = std::make_shared<MMU>();
    cpu = new CPU(mmu);
    gpu = new GPU(mmu);
    h_interrupt = new InterruptHandler(mmu);
    h_timer = new Timer(mmu);
    loadMemory("../bin/Tetris.gb");
    gpu->initGraphics();
}

int GameBoy::handleKeyEvent(SDL_Event &e)
{
    int key = -1;
    switch (e.key.keysym.sym)
    {
    case SDLK_RIGHT:
        key = 0;
        break;
    case SDLK_LEFT:
        key = 1;
        break;
    case SDLK_UP:
        key = 2;
        break;
    case SDLK_DOWN:
        key = 3;
        break;
    case SDLK_a:
        key = 4;
        break;
    case SDLK_s:
        key = 5;
        break;
    case SDLK_SPACE:
        key = 6;
        break;
    case SDLK_RETURN:
        key = 7;
        break;
    default:
        break;
    }
    return key;
}

void GameBoy::handleKeyPressed(int key)
{
    // TODO: finish
    joypad &= ~(1 << key);
}

void GameBoy::handleKeyReleased(int key)
{
    joypad |= (1 << key);
}

void GameBoy::handleInput(SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
    {
        int key = handleKeyEvent(e);
        if (key != -1)
        {
            if (e.type == SDL_KEYDOWN)
            {
                handleKeyPressed(key);
            }
            else
            {
                handleKeyReleased(key);
            }
        }
    }
}

void GameBoy::emulate()
{
    // TODO: throttle the emulation
    if (total_cycles < MAX_CYCLES)
    {
        cycles = cpu->tick(); // step the CPU
        total_cycles += cycles;
        gpu->tick(cycles); // step the GPU
        // handle timers
        if (h_timer->clockEnabled())
            h_timer->handleTimers(cycles, h_interrupt);
        // do interrupts
        if (mmu->interruptsEnabled())
            h_interrupt->handleInterrupts(cpu);
    }
    else
        total_cycles = 0;
}
