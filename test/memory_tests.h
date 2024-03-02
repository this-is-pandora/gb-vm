#include <gtest/gtest.h>
#include "../include/memory.h"

class MEM_TEST : public testing::Test
{
protected:
    void SetUp() override
    {
        mmu = new MMU();
    }
    MMU *mmu;
};

// read & writing
TEST_F(MEM_TEST, READ_MEM)
{
    ASSERT_EQ(mmu->readByte(0x0), 0);
}

TEST_F(MEM_TEST, WRITE_MEM)
{
    mmu->writeByte(0x0, 100);
    uint8_t value = mmu->readByte(0x0);
    ASSERT_EQ(value, 100);
}

// push & pop
TEST_F(MEM_TEST, PUSH_STACK)
{
}
TEST_F(MEM_TEST, POP_STACK)
{
}

// DMA
TEST_F(MEM_TEST, DMA)
{
}

// memory banking
