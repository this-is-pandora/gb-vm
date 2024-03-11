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
    ASSERT_EQ(mmu->readByte(0xFF11), 0xBF);
}

TEST_F(MEM_TEST, WRITE_MEM)
{
    mmu->writeByte(0xC050, 100);
    uint8_t value = mmu->readByte(0xC050);
    ASSERT_EQ(value, 100);
}

// push & pop
TEST_F(MEM_TEST, PUSH_POP_STACK)
{
    uint16_t sp = 0xFFFE;
    mmu->push(0x100, sp);
    ASSERT_EQ(sp, 0xFFFC);
    uint16_t val = mmu->pop(sp);
    ASSERT_EQ(val, 0x100);
    ASSERT_EQ(sp, 0xFFFE);
}

/* write to ECHO RAM */
TEST_F(MEM_TEST, WRITE_ECHORAM)
{
    mmu->writeByte(0xE000, 0x10);
    uint8_t val = mmu->readByte(0xC000);
    ASSERT_EQ(val, 0x10);
}

/* DMA
TEST_F(MEM_TEST, DMA)
{
} */

// load ROM

// memory banking