#include "test_regs.h"
#include "test_mem.h"
#include "test_flags.h"
#include "test_ops.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}