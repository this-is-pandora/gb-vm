cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_C_COMPILER "/usr/bin/gcc.exe")

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gbvm)

find_package(SDL2 REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  )

  #add_subdirectory(test)

file(GLOB SOURCES 
  "${PROJECT_SOURCE_DIR}/include/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(gbvm ${SOURCES})
target_link_libraries(gbvm ${SDL2_LIBRARIES})