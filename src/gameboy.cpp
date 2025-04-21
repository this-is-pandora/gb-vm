#include "gameboy.h"
#include <iostream>
#include <thread>

GameBoy::GameBoy()
{
    cycles = 0;
    total_cycles = 0;
    unpaused = true;
}

GameBoy::~GameBoy()
{
    // delete cpu;
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
        mmu->writeByte(0xFF00, joypad);
    }
}
void GameBoy::handleInterrupts()
{
    if (mmu->interruptsEnabled())
    {
        if (mmu->readByte(IE) & mmu->readByte(IER))
        {
            uint16_t _pc, _sp;
            // v-blank
            if (h_interrupt->readInterrupt(VBLANK))
            {
                _pc = h_interrupt->serviceInterrupt(VBLANK);
                _sp = cpu->getSP();
                mmu->push(cpu->getPC(), _sp);
                cpu->setSP(_sp);
                cpu->setPC(_pc);
            }
            // lcd
            if (h_interrupt->readInterrupt(LCD))
            {
                _pc = h_interrupt->serviceInterrupt(LCD);
                _sp = cpu->getSP();
                mmu->push(cpu->getPC(), _sp);
                cpu->setSP(_sp);
                cpu->setPC(_pc);
            }
            // timer
            if (h_interrupt->readInterrupt(TIMER))
            {
                _pc = h_interrupt->serviceInterrupt(TIMER);
                _sp = cpu->getSP();
                mmu->push(cpu->getPC(), _sp);
                cpu->setSP(_sp);
                cpu->setPC(_pc);
            }
            // joypad
            if (h_interrupt->readInterrupt(JOYPAD))
            {
                _pc = h_interrupt->serviceInterrupt(JOYPAD);
                _sp = cpu->getSP();
                mmu->push(cpu->getPC(), _sp);
                cpu->setSP(_sp);
                cpu->setPC(_pc);
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
        gpu->tick(cycles);           // step the GPU
        if (h_timer->clockEnabled()) // handle timers
            h_timer->handleTimers(cycles);
        handleInterrupts(); // do interrupts
    }
    else
        total_cycles = 0;
}
