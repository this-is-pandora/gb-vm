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

void InterruptHandler::serviceInterrupt(int id, CPU *cpu)
{
    // disable interrupt
    // change IER so cpu knows interrupt has been serviced
    mmu->enableInterrupts(false);
    uint8_t reqs = mmu->readByte(IER);
    reqs &= ~(1 << id);
    mmu->writeByte(IER, reqs);

    uint16_t pc = cpu->getPC();
    uint16_t sp = cpu->getSP();
    mmu->push(pc, sp);

    switch (id)
    {
    case VBLANK:
        cpu->setPC(0x40);
        break;
    case LCD:
        cpu->setPC(0x48);
        break;
    case TIMER:
        cpu->setPC(0x50);
        break;
    case JOYPAD:
        cpu->setPC(0x60);
        break;
    default:
        break;
    }
}

void InterruptHandler::handleInterrupts(CPU *cpu)
{

    if (mmu->interruptsEnabled())
    {
        if (mmu->readByte(IE) & mmu->readByte(IER))
        {
            // v-blank
            if (readInterrupt(VBLANK))
            {
                serviceInterrupt(VBLANK, cpu);
            }
            // lcd
            if (readInterrupt(LCD))
            {
                serviceInterrupt(LCD, cpu);
            }
            // timer
            if (readInterrupt(TIMER))
            {
                serviceInterrupt(TIMER, cpu);
            }
            // joypad
            if (readInterrupt(JOYPAD))
            {
                serviceInterrupt(JOYPAD, cpu);
            }
        }
    }
}