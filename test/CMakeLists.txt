enable_testing()
set(SOURCES
    main_tests.cpp
    register_tests.h
    memory_tests.h
    ../src/opcodes.cpp
    ../src/cpu.cpp
    ../include/cpu.h
    ../src/memory.cpp
    ../include/memory.h
)

add_executable(main_tests ${SOURCES})

target_link_libraries(main_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(main_tests)