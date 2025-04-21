#include "interrupts.h"

InterruptHandler::InterruptHandler(std::shared_ptr<MMU> mmu) : mmu(mmu)
{
    // mmu = mmu;
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

uint8_t InterruptHandler::serviceInterrupt(int id)
{
    // disable interrupt
    // change IER so cpu knows interrupt has been serviced
    uint8_t pc = 0x0;
    mmu->enableInterrupts(false);
    uint8_t reqs = mmu->readByte(IER);
    reqs &= ~(1 << id);
    mmu->writeByte(IER, reqs);
    // mmu->push(cpu->pc, cpu->sp);
    switch (id)
    {
    case VBLANK:
        pc = 0x40;
        break;
    case LCD:
        pc = 0x48;
        break;
    case TIMER:
        pc = 0x50;
        break;
    case JOYPAD:
        pc = 0x60;
        break;
    default:
        break;
    }
    return pc;
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