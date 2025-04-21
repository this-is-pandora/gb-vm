enable_testing()
set(SOURCES
    test_main.cpp
    test_regs.h
    test_flags.h
    test_mem.h
    test_ops.h
    ../src/opcodes.cpp
    ../src/cpu.cpp
    ../src/memory.cpp
    ../src/interrupts.cpp
    ../src/timer.cpp
    ../include/cpu.h
    ../include/memory.h
    ../include/interrupts.h
    ../include/timer.h
)

add_executable(tests ${SOURCES})

target_link_libraries(tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)