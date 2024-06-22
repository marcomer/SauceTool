cmake_minimum_required(VERSION 3.12)


# ==============
# Build Start
# ============== 

project("Sauce")

set(CMAKE_C_STANDARD "C99")

# compiling options/flags
# COMPILE_OPTIONS_LIST = default options for all builds
set(COMPILE_OPTIONS_LIST -m64 -Wall)
set(DEBUG_COMPILE_OPTIONS -g)
set(RELEASE_COMPILE_OPTIONS -O3)

add_executable(${EXE_NAME} ${SRC_FILES})

# TODO: How do the debug/release flags work again?
target_compile_options(${EXE_NAME} PRIVATE ${COMPILE_OPTIONS_LIST})
target_compile_options(${EXE_NAME} PRIVATE $<$<CONFIG:DEBUG>:${DEBUG_COMPILE_OPTIONS}>)
target_compile_options(${EXE_NAME} PRIVATE $<$<CONFIG:RELEASE>:${RELEASE_COMPILE_OPTIONS}>)

target_include_directories(
    ${EXE_NAME} 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    ${INCLUDE_DIR_LIST}
)
# TODO: does it look through the folder recursively?

target_link_directories(
    ${EXE_NAME}
    PUBLIC "/usr/local/lib/"
)
# TODO: make sure this works on windows and mac

target_link_libraries(
    ${EXE_NAME}
    ${LINK_LIB_LIST}
)


