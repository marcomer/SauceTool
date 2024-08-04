# /test/CMakeLists.txt

# Add a custom target that copies the expect/ directory to the test binary directory
add_custom_target(copy_expect ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/test/expect ${CMAKE_CURRENT_BINARY_DIR}/expect
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add a custom target that will depend on all tests being built
add_custom_target(run_all_tests ALL)

# Create the actual/ directory in the test binary directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/actual)

# Create all the "actual" files written to by the test suites
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/comment_remove_actual.ans)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/comment_write_actual.ans)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/remove_actual.ans)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/actual/write_actual.ans)


# sauce_tool_add_test() function
# Create a test given a testname
function(sauce_tool_add_test testname)
  if(${ARGC} EQUAL 0)
    message(FATAL_ERROR "sauce_tool_add_test() must be given a test name")
  endif()

  # create the test suite
  add_executable(${testname}
    "src/${testname}.c" 
    src/TestRes.c
  )
  target_link_libraries(${testname}
    SauceTool
    Unity
  )
  add_test(
    NAME ${testname}
    COMMAND ${testname}
  )
  
  # make sure expect data files are copied before test suite is built
  add_dependencies(${testname} copy_expect)

  # require run_all_tests to wait for test suite to build
  add_dependencies(run_all_tests ${testname})
endfunction()


# Add tests
sauce_tool_add_test(HelperFunctionTest)
sauce_tool_add_test(RecordReadTest)
sauce_tool_add_test(RecordWriteTest)
sauce_tool_add_test(RecordRemoveTest)
sauce_tool_add_test(CommentReadTest)
sauce_tool_add_test(CommentWriteTest)
sauce_tool_add_test(CommentRemoveTest)
sauce_tool_add_test(CheckTest)

# Create command such that all tests are ran with ctest when all test suites are built
add_custom_command(
  TARGET run_all_tests
  COMMENT "All tests built, running all tests..."
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> --output-on-failure
)
