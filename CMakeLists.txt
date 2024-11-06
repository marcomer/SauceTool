cmake_minimum_required(VERSION 3.29)

set(CMAKE_C_STANDARD "99")

project("SauceTool" C)

include(GNUInstallDirs)

# Create main SAUCE library
add_library(SauceTool STATIC
  src/SauceTool.c
)

# Enable full warnings
target_compile_options(SauceTool PRIVATE
  $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
    -Wall>
  $<$<C_COMPILER_ID:MSVC>:
    /W4>
)

# Add headers as file set
target_sources(SauceTool PUBLIC 
  FILE_SET HEADERS
  BASE_DIRS "include/"
  FILES 
    "include/SauceTool.h"
)



# ========= Install =========

set(SAUCETOOL_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/SauceTool")

# Create install instructions
install(TARGETS SauceTool 
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET HEADERS DESTINATION "${SAUCETOOL_INSTALL_INCLUDEDIR}"
)

# On install, generate actual cmake_uninstall.cmake script
install(CODE "
  set(SAUCETOOL_INSTALL_INCLUDEDIR ${SAUCETOOL_INSTALL_INCLUDEDIR})
  configure_file(${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in cmake_uninstall.cmake @ONLY)
")

# create uninstall target which runs the uninstall script
add_custom_target(uninstall 
  COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)



# ========== Testing ============

# Create Unity C testing framework library
add_library(Unity STATIC
  Unity/src/unity.c
)

target_include_directories(Unity PUBLIC
  Unity/src
)

# Initialize testing
include(CTest)
add_subdirectory(test)
