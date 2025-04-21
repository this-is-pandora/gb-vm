#define protected public
#define private public
#include <gtest/gtest.h>
#include "../include/cpu.h"
#include "../include/memory.h"
#include <iostream>
#undef protected
#undef private

class OP_TEST : public testing::Test
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

// load an 8-bit value into register A
TEST_F(OP_TEST, LD_A)
{
      // cpu->execute();
}

TEST_F(OP_TEST, ADD_8BIT)
{
}

TEST_F(OP_TEST, ADD_16BIT)
{
}

TEST_F(OP_TEST, SUB_8BIT)
{
}

TEST_F(OP_TEST, SUB_16BIT)
{
}

// DEC B
TEST_F(OP_TEST, DEC_B)
{
      // cpu->execute(0x05);
      ASSERT_EQ(cpu->getB(), 0);
}

TEST_F(OP_TEST, INC_B)
{
}

TEST_F(OP_TEST, AND_8BIT)
{
}
TEST_F(OP_TEST, AND_16BIT)
{
}
TEST_F(OP_TEST, XOR_8BIT)
{
}

// JR NZ, *
TEST_F(OP_TEST, JR_NZ_IMM)
{
      // cpu->execute(0x20);
      //  TODO
}

// LD (HL-), A
TEST_F(OP_TEST, LD_HLD_A)
{
      // TODO
}

/* test stack methods */
// extended instructions