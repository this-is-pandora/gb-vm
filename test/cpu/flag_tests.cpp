#include <gtest/gtest.h>
#include "include/cpu.h"

class FLAG_TEST : public testing::Test
{
protected:
    void SetUp() override
    {
        cpu = new CPU();
    }
    CPU *cpu;
};

// flag = 0xB0 or 0b10110000
// TODO: read all flags
TEST_F(FLAG_TEST, READ_F)
{
    EXPECT_EQ(0, 0);
}

// 0-flag (z)
TEST_F(FLAG_TEST, READ_Z_FLAG)
{
    bool val = cpu->getZeroFlag();
    EXPECT_EQ(val, true);
}
TEST_F(FLAG_TEST, SET_Z_FLAG)
{
    cpu->setZeroFlag(false);
    bool val = cpu->getZeroFlag();
    EXPECT_EQ(val, false);
}

// subtract flag (n)
TEST_F(FLAG_TEST, READ_N_FLAG)
{
    bool val = cpu->getSubtractFlag();
    EXPECT_EQ(val, false);
}
TEST_F(FLAG_TEST, SET_N_FLAG)
{
    cpu->setSubtractFlag(true);
    bool val = cpu->getSubtractFlag();
    EXPECT_EQ(val, true);
}

// half-carry flag (h)
TEST_F(FLAG_TEST, READ_H_FLAG)
{
    bool val = cpu->getHalfCarryFlag();
    EXPECT_EQ(val, true);
}
TEST_F(FLAG_TEST, SET_H_FLAG)
{
    cpu->setHalfCarryFlag(false);
    uint8_t val = cpu->getHalfCarryFlag();
    EXPECT_EQ(val, false);
}

// carry flag (c)
TEST_F(FLAG_TEST, READ_C_FLAG)
{
    bool val = cpu->getCarryFlag();
    EXPECT_EQ(val, true);
}
TEST_F(FLAG_TEST, SET_C_FLAG)
{
    cpu->setCarryFlag(false);
    bool val = cpu->getCarryFlag();
    EXPECT_EQ(val, false);
}