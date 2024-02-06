#include <gtest/gtest.h>
#include "cpu.h"
#include "memory.h"

class CPU_TEST : public testing::Test
{
protected:
    void SetUp() override
    {
        memory = new MMU();
        cpu = new CPU(memory);
    }

    CPU *cpu;
    MMU *memory;
};

// load 16-bit value into register AF
TEST_F(CPU_TEST, LD_AF_NN)
{
    EXPECT_EQ(0, 0);
}
// read register AF
TEST_F(CPU_TEST, READ_AF1)
{
    EXPECT_EQ(0, 0);
}
// read register A
TEST_F(CPU_TEST, READ_A1)
{
    EXPECT_EQ(0, 0);
}
// read register F
TEST_F(CPU_TEST, READ_F1)
{
    EXPECT_EQ(0, 0);
}

// add a value to register F
TEST_F(CPU_TEST, ADD_F)
{
    EXPECT_EQ(0, 0);
}
// read register F
TEST_F(CPU_TEST, READ_F2)
{
    EXPECT_EQ(0, 0);
}
// read register AF
TEST_F(CPU_TEST, READ_AF2)
{
    EXPECT_EQ(0, 0);
}

/* TODO: push & pop tests */

/* TODO: flag tests */

/* TODO: instruction tests */