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


# Initialize RecordWriteTest suite
add_executable(RecordWriteTest
  src/RecordWriteTest.c
  src/TestRes.c
)
target_link_libraries(RecordWriteTest
  Sauce
  Unity
)
add_test(
  NAME RecordWriteTest
  COMMAND RecordWriteTest
)


# Initialize RecordRemoveTest suite
add_executable(RecordRemoveTest
  src/RecordRemoveTest.c
  src/TestRes.c
)
target_link_libraries(RecordRemoveTest
  Sauce
  Unity
)
add_test(
  NAME RecordRemoveTest
  COMMAND RecordRemoveTest
)


# Initialize CommentReadTest suite
add_executable(CommentReadTest
  src/CommentReadTest.c
  src/TestRes.c
)
target_link_libraries(CommentReadTest
  Sauce
  Unity
)
add_test(
  NAME CommentReadTest
  COMMAND CommentReadTest
)


# Initialize CommentWriteTest suite
add_executable(CommentWriteTest
  src/CommentWriteTest.c
  src/TestRes.c
)
target_link_libraries(CommentWriteTest
  Sauce
  Unity
)
add_test(
  NAME CommentWriteTest
  COMMAND CommentWriteTest
)


# Initialize CommentRemoveTest suite
add_executable(CommentRemoveTest
  src/CommentRemoveTest.c
  src/TestRes.c
)
target_link_libraries(CommentRemoveTest
  Sauce
  Unity
)
add_test(
  NAME CommentRemoveTest
  COMMAND CommentRemoveTest
)


# Initialize CheckTest suite
add_executable(CheckTest
  src/CheckTest.c
  src/TestRes.c
)
target_link_libraries(CheckTest
  Sauce
  Unity
)
add_test(
  NAME CheckTest
  COMMAND CheckTest
)



# Add a custom target that copies the expect/ directory to the test binary directory
add_custom_target(copy_expect ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/test/expect ${CMAKE_CURRENT_BINARY_DIR}/expect
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Ensure that tests have access to expect/ copy
add_dependencies(HelperFunctionTest copy_expect)
add_dependencies(RecordReadTest copy_expect)
add_dependencies(RecordWriteTest copy_expect)
add_dependencies(RecordRemoveTest copy_expect)
add_dependencies(CommentReadTest copy_expect)
add_dependencies(CommentWriteTest copy_expect)
add_dependencies(CommentRemoveTest copy_expect)
add_dependencies(CheckTest copy_expect)

# Create the actual/ directory in the test binary directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/actual)

# Create all the "actual" files written to by the test suites
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/comment_remove_actual.ans)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/comment_write_actual.ans)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/remove_actual.ans)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/write_actual.ans)
