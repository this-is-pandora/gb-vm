#include "../include/interrupts.h"

InterruptHandler::InterruptHandler(std::shared_ptr<MMU> mmu, CPU *_cpu) : mmu(mmu)
{
    // mmu = mmu;
    cpu = _cpu;
}

bool InterruptHandler::readInterrupt(Interrupts i)
{
    return ((mmu->readByte(IE) & i) & (mmu->readByte(IER) & i)) ? true : false;
}
// TODO: check
void InterruptHandler::requestInterrupt(int id)
{
    uint8_t data = mmu->readByte(IER);
    data |= (1 << id);
    mmu->writeByte(IER, data);
}

void InterruptHandler::serviceInterrupt(int id)
{
    // disable interrupt
    // change IER so cpu knows interrupt has been serviced
    mmu->enableInterrupts(false);
    uint8_t reqs = mmu->readByte(IER);
    reqs &= ~(1 << id);
    mmu->writeByte(IER, reqs);
    mmu->push(cpu->pc, cpu->sp);
    switch (id)
    {
    case VBLANK:
        cpu->pc = 0x40;
        break;
    case LCD:
        cpu->pc = 0x48;
        break;
    case TIMER:
        cpu->pc = 0x50;
        break;
    case JOYPAD:
        cpu->pc = 0x60;
        break;
    default:
        break;
    }
}

void InterruptHandler::handleInterrupts()
{

    if (mmu->interruptsEnabled())
    {
        if (mmu->readByte(IE) & mmu->readByte(IER))
        {
            // v-blank
            if (readInterrupt(VBLANK))
            {
                serviceInterrupt(VBLANK);
            }
            // lcd
            if (readInterrupt(LCD))
            {
                serviceInterrupt(LCD);
            }
            // timer
            if (readInterrupt(TIMER))
            {
                serviceInterrupt(TIMER);
            }
            // joypad
            if (readInterrupt(JOYPAD))
            {
                serviceInterrupt(JOYPAD);
            }
        }
    }
}