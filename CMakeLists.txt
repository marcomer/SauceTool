cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD "99")

project("SauceTool" C)

# Add main SAUCE library
add_library(SauceTool STATIC
  src/SauceTool.c
)

target_include_directories(SauceTool PUBLIC
  "include/"
)


# Add Unity C testing framework
add_library(Unity STATIC
  Unity/src/unity.c
)

target_include_directories(Unity PUBLIC
  Unity/src
)


# Initialize testing
include(CTest)
add_subdirectory(test)
