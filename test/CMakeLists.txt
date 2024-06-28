# /test/CMakeLists.txt

# Initialize HelperFunctionTest suite
add_executable(HelperFunctionTest
  src/HelperFunctionTest.c 
  src/TestRes.c
)
target_link_libraries(HelperFunctionTest
  Sauce
  Unity
)
add_test(
  NAME HelperFunctionTest
  COMMAND HelperFunctionTest
)

# Initialize RecordReadTest suite
add_executable(RecordReadTest
  src/RecordReadTest.c
  src/TestRes.c
)
target_link_libraries(RecordReadTest
  Sauce
  Unity
)
add_test(
  NAME RecordReadTest
  COMMAND RecordReadTest
)



# Add a custom target that copies the expect/ directory to the test binary directory
add_custom_target(copy_expect ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/test/expect ${CMAKE_CURRENT_BINARY_DIR}/expect
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)