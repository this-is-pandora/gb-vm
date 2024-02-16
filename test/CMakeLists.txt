enable_testing()
set(SOURCES 
    register_tests.cpp
    ../src/opcodes.cpp
    ../src/cpu.cpp
    ../include/cpu.h
    ../src/memory.cpp
    ../include/memory.h
)

add_executable(register_tests ${SOURCES})
target_link_libraries(register_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(register_tests)