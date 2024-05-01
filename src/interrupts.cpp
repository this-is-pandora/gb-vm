#include "../include/interrupts.h"

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

void InterruptHandler::serviceInterrupt(int id, uint16_t &pc, uint16_t &sp)
{
    // disable interrupt
    // change IER so cpu knows interrupt has been serviced
    mmu->enableInterrupts(false);
    uint8_t reqs = mmu->readByte(IER);
    reqs &= ~(1 << id);
    mmu->writeByte(IER, reqs);
    mmu->push(pc, sp);
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
}

void InterruptHandler::handleInterrupts(uint16_t &pc, uint16_t &sp)
{

    if (mmu->interruptsEnabled())
    {
        if (mmu->readByte(IE) & mmu->readByte(IER))
        {
            // v-blank
            if (readInterrupt(VBLANK))
            {
                serviceInterrupt(VBLANK, pc, sp);
            }
            // lcd
            if (readInterrupt(LCD))
            {
                serviceInterrupt(LCD, pc, sp);
            }
            // timer
            if (readInterrupt(TIMER))
            {
                serviceInterrupt(TIMER, pc, sp);
            }
            // serial
            /*if (readInterrupt(SERIAL))
            {
                serviceInterrupt(SERIAL, pc, sp);
            }*/
            // joypad
            if (readInterrupt(JOYPAD))
            {
                serviceInterrupt(JOYPAD, pc, sp);
            }
        }
    }
}