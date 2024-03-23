#include "register_tests.h"
#include "memory_tests.h"
#include "flag_tests.h"
#include "instruction_tests.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}