cmake_minimum_required(VERSION 3.14)
project(gb-vm)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# link to SDL2
set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/SDL2/include/SDL2)
set(SDL2_LIB_DIR ${CMAKE_SOURCE_DIR}/SDL2/lib/)
include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

# testing stuff
include(CTest)
include_directories(include)
add_subdirectory(test)



set(SOURCES
    ../src/main.cpp
    ../src/gameboy.cpp
    ../src/graphics.cpp
    ../src/opcodes.cpp
    ../src/cpu.cpp
    ../src/memory.cpp
    ../src/interrupts.cpp
    ../src/timer.cpp
    ../include/cpu.h
    ../include/graphics.h
    ../include/memory.h
    ../include/interrupts.h
    ../include/timer.h
    ../include/gameboy.h

)

add_executable(gbvm ${SOURCES} ${INCLUDE_DIRECTORIES})
target_link_libraries(gbvm mingw32 SDL2main SDL2)

