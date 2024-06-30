#ifndef SAUCE_TEST_RES_HEADER_INCLUDED
#define SAUCE_TEST_RES_HEADER_INCLUDED
#include <stdio.h>
#include "Sauce.h"

// Basic files for reading. These files should not be changed

// TestFile1 -> ANSI file with SAUCE record and comment
#define SAUCE_TESTFILE1_PATH    "expect/TestFile1.ans"

// TestFile2 -> File only containing a SAUCE record
#define SAUCE_TESTFILE2_PATH    "expect/TestFile2.ans"

// No Sauce File -> File does not contain any SAUCE info
#define SAUCE_NOSAUCE_PATH      "expect/NoSauce.txt"

// ShortFile -> File under 128 bytes
#define SAUCE_SHORTFILE_PATH    "test/res/ShortFile.txt"


// Expected write file results. These files should not be changed

// Result of writing to an empty file
#define SAUCE_WRITETOEMPTY_PATH   "expect/write/WriteToEmpty.txt"

// Result of replacing TestFile1's SAUCE record
#define SAUCE_REPLACE_PATH        "expect/write/ReplaceExpect.txt"

// Result of appending to a file
#define SAUCE_APPEND_PATH         "expect/write/AppendExpect.txt"

// File to contain the actual results of a test write
#define SAUCE_WRITE_ACTUAL_PATH   "expect/actual/write_actual.txt"

// The expected result of SAUCE_set_default
extern const SAUCE default_record;


SAUCE* test_get_testfile1_expected_record();

SAUCE_CommentBlock* test_get_testfile1_expected_comment();

SAUCE* test_get_testfile2_expected_record();




// Test if a file's contents match another file's contents.
// Return true on a complete match, false if otherwise.
int test_file_matches_expected(const char* actual_filepath, const char* expected_filepath);

// Test if a buffer's contents match another file's contents.
// Return true on a complete match, false if otherwise.
int test_buffer_matches_expected(const char* buffer, uint32_t n, const char* expected_filepath);

// Copy a file from a src file to a dest file.
int copy_file(const char* src, const char* dest);

#endif //SAUCE_TEST_RES_HEADER_INCLUDED