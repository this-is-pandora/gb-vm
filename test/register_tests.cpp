#include <gtest/gtest.h>
#include "cpu.h"

class REG_TEST : public testing::Test
{
protected:
    void SetUp() override
    {
        AF.value = 0x01B0; //
    }
    Register AF;
};

// check register values
TEST_F(REG_TEST, CHECK_VAL_AF)
{
    EXPECT_EQ(AF.value, 0x01B0);
}

TEST_F(REG_TEST, CHECK_VAL_A)
{
    EXPECT_EQ(AF.hi, 0x01);
}

TEST_F(REG_TEST, CHECK_VAL_F)
{
    EXPECT_EQ(AF.lo, 0xB0);
}

// change value of A
TEST_F(REG_TEST, CHG_A)
{
    AF.hi = 0x20;
    EXPECT_EQ(AF.value, 0x20B0);
}

// increment register
TEST_F(REG_TEST, INC_AF)
{
    AF.value += 1;
    EXPECT_EQ(AF.value, 0x01B1);
}

// increment register A
TEST_F(REG_TEST, INC_A)
{
    AF.hi += 1;
    EXPECT_EQ(AF.value, 0x02B0);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}