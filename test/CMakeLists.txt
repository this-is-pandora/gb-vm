enable_testing()
set(SOURCES
    main_tests.cpp
    register_tests.h
    flag_tests.h
    memory_tests.h
    instruction_tests.h
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

add_executable(main_tests ${SOURCES})

target_link_libraries(main_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(main_tests)