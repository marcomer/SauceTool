cmake_minimum_required(VERSION 3.12)

project("SAUCE" C)

set(CMAKE_C_STANDARD "99")

# Add main SAUCE library
add_library(Sauce STATIC
  src/SauceTool.c
)

target_include_directories(Sauce PUBLIC
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
