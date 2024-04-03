#include <gtest/gtest.h>
#include "../include/cpu.h"
#include "../include/memory.h"

class INSTRCTN_TEST : public testing::Test
{
protected:
      void SetUp() override
      {
            // mmu = new MMU();
            // cpu = new CPU(mmu);
      }
      // MMU *mmu;
      // CPU *cpu;
};

/* Tests individual instructions, e.g. LD, ADD, etc. */

/* basic program that loads some values into several registers
then performs some arithmetic ops */
TEST_F(INSTRCTN_TEST, BASIC_PRGRM)
{
}

/* test stack methods */
// extended instructions