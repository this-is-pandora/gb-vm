#include <gtest/gtest.h>
#include "include/memory.h"

class MEM_TEST : public testing::Test
{
protected:
    void SetUp() override
    {
        memory = new MMU();
    }

    MMU *memory;
};

// read & writing
// push & pop
// DMA
// memory banking