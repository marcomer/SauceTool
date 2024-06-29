#ifndef SAUCE_TEST_RES_HEADER_INCLUDED
#define SAUCE_TEST_RES_HEADER_INCLUDED
#include <stdio.h>
#include "Sauce.h"

// TestFile1 -> ANSI file with SAUCE record and comment
#define SAUCE_TESTFILE1_PATH    "expect/TestFile1.ans"

// TestFile2 -> File only containing a SAUCE record
#define SAUCE_TESTFILE2_PATH    "expect/TestFile2.ans"

// No Sauce File -> File does not contain any SAUCE info
#define SAUCE_NOSAUCE_PATH      "expect/NoSauce.txt"

// ShortFile -> File under 128 bytes
#define SAUCE_SHORTFILE_PATH    "test/res/ShortFile.txt"

// The expected result of SAUCE_set_default
extern const SAUCE default_record;


SAUCE* test_get_testfile1_expected_record();

SAUCE_CommentBlock* test_get_testfile1_expected_comment();

SAUCE* test_get_testfile2_expected_record();




// Test if a file's contents match another file's contents.
// Return true on a complete match, false if otherwise.
int test_file_matches_expected(FILE* actual, const char* expected_filepath);

// Test if a buffer's contents match another file's contents.
// Return true on a complete match, false if otherwise.
int test_buffer_matches_expected(const char* buffer, uint32_t n, const char* expected_filepath);


#endif //SAUCE_TEST_RES_HEADER_INCLUDED