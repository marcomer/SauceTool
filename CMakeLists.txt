cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD "99")

project("SauceTool" C)

include(GNUInstallDirs)

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

install(FILES "include/SauceTool.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(TARGETS SauceTool 
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

# Initialize testing
include(CTest)
add_subdirectory(test)
