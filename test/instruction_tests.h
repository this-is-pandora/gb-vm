#include <gtest/gtest.h>
#include "../include/cpu.h"
#include "../include/memory.h"
#include <iostream>

class INSTRCTN_TEST : public testing::Test
{
protected:
      void SetUp() override
      {
            // mmu = new MMU();
            mmu = std::make_shared<MMU>();
            cpu = new CPU(mmu);
      }
      // MMU *mmu;
      std::shared_ptr<MMU> mmu;
      CPU *cpu;
      // CPU *cpu;
};

/* Tests individual instructions, e.g. LD, ADD, etc. */

// booting code: load 0xFFFE to SP
TEST_F(INSTRCTN_TEST, BOOTING)
{
      int cycles;
      mmu->loadBootROM();   // load boot rom to memory
      cycles = cpu->tick(); // execute the first boot rom instruction
      uint16_t pc = cpu->getPC();
      uint16_t sp = cpu->getSP();
      /*
       * the pc and sp are off
       * TODO: the correct values after running 1 clock cycle should be:
       * pc = 0x03, sp = 0xFFFE
       * but pc = 0x02 and sp = 0
       */
      ASSERT_EQ(pc, 0x03);
      ASSERT_EQ(sp, 0xFFFE);
      ASSERT_EQ(cycles, 12);
      cycles = cpu->tick();
      ASSERT_EQ(0x04, cpu->getPC());
}

/*
// fetching opcode
TEST_F(INSTRCTN_TEST, FETCHING)
{
      // cpu->tick();
}

// register to register
TEST_F(INSTRCTN_TEST, LD_1)
{
}

// memory to register
TEST_F(INSTRCTN_TEST, LD_2) {}

// ALU
TEST_F(INSTRCTN_TEST, ALU_1)
{
} */

/* test stack methods */
// extended instructions