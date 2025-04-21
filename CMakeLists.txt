cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")
set(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gbvm)

#[[include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
]]

# link to SDL2
set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/SDL2/include/SDL2)
set(SDL2_LIB_DIR ${CMAKE_SOURCE_DIR}/SDL2/lib/)
#include(CTest)

include_directories(
  ${SDL2_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  )

link_directories(${SDL2_LIB_DIR})
#add_subdirectory(test)

file(GLOB SOURCES 
  "${PROJECT_SOURCE_DIR}/include/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(gbvm ${SOURCES})
target_include_directories(gbvm PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(gbvm mingw32 SDL2main SDL2)

