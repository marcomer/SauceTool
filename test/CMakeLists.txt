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