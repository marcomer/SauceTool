#ifndef SAUCE_TEST_RES_HEADER_INCLUDED
#define SAUCE_TEST_RES_HEADER_INCLUDED
#include <stdio.h>
#include "Sauce.h"

// Basic files for reading. These files should not be changed

// TestFile1 -> ANSI file with SAUCE record and comment
#define SAUCE_TESTFILE1_PATH    "expect/TestFile1.ans"

// TestFile2 -> File only containing a SAUCE record
#define SAUCE_TESTFILE2_PATH    "expect/TestFile2.ans"

// TestFile3 -> File containing original file contents and a SAUCE record (w/ no comment)
#define SAUCE_TESTFILE3_PATH    "expect/TestFile3.ans"

// No Sauce File -> File does not contain any SAUCE info
#define SAUCE_NOSAUCE_PATH      "expect/NoSauce.txt"

// ShortFile -> File under 128 bytes
#define SAUCE_SHORTFILE_PATH    "expect/ShortFile.txt"

// SauceButNoEOF -> TestFile1 with a missing EOF character
#define SAUCE_SAUCEBUTNOEOF_PATH  "expect/SauceButNoEOF.ans"

// CommentButNoRecord -> TestFile1 without a SAUCE record
#define SAUCE_COMMENTBUTNORECORD_PATH   "expect/CommentButNoRecord.ans"

// InvalidComment -> Long file with an incorrect Comments field
#define SAUCE_INVALIDCOMMENT_PATH       "expect/InvalidComment.txt"

// LongNoSauce -> A CMakeLists.txt file with no SAUCE info
#define SAUCE_LONGNOSAUCE_PATH          "expect/LongNoSauce.txt"

// OnlyRecord -> Record from TestFile2.ans but with no EOF character
#define SAUCE_ONLYRECORD_PATH           "expect/OnlyRecord.ans"


// Expected write file results. These files should not be changed

// Result of writing to an empty file
#define SAUCE_WRITETOEMPTY_PATH   "expect/write/WriteToEmptyExpect.txt"

// Result of replacing TestFile1's SAUCE record
#define SAUCE_REPLACE_PATH        "expect/write/ReplaceExpect.txt"

// Result of appending to a file
#define SAUCE_APPEND_PATH         "expect/write/AppendExpect.txt"

// File to contain the actual results of a test write
#define SAUCE_WRITE_ACTUAL_PATH   "actual/write_actual.txt"


// Expected remove file results. These files should not be changed

// Result of removing a SAUCE record from TestFile3.ans
#define SAUCE_REMOVE_ONLY_RECORD_PATH "expect/remove/RemoveOnlyRecord.txt"

// Result of removing a SAUCE record and CommentBlock from TestFile1.ans
#define SAUCE_REMOVE_RECORD_AND_COMMENT_PATH  "expect/remove/RemoveBoth.txt"

// File to contain the actual results of a test remove
#define SAUCE_REMOVE_ACTUAL_PATH  "actual/remove_actual.txt"


// Expected comment write file results. These files should not be changed.

// File to contain the acutal results of a test comment write
#define SAUCE_COMMENT_WRITE_ACTUAL_PATH     "actual/comment_write_actual.txt"

// AddCommentToRecord -> Contains the result of adding shortComment to TestFile2.ans
#define SAUCE_ADDCOMMENTTORECORD_PATH       "expect/comment_write/AddCommentToRecord.txt"

// ReplaceExistingComment -> Contains the result of replacing TestFile1's comment with longComment
#define SAUCE_REPLACEEXISTINGCOMMENT_PATH   "expect/comment_write/ReplaceExistingComment.txt"

// AddCommentAndEOFToRecord -> Contains the result of adding shortComment and an EOF to OnlyRecord.ans
#define SAUCE_ADDCOMMENTANDEOFTORECORD_PATH   "expect/comment_write/AddCommentAndEOFToRecord.txt"

// ReplaceCommentAndAddEOF -> Contains the result of replacing SauceButNoEOF's comment with shortComment and adding EOF
#define SAUCE_REPLACECOMMENTANDADDEOF_PATH    "expect/comment_write/ReplaceCommentAndAddEOF.txt"



// Expected comment remove file results. These files should not be changed

// File to contain the actual result of a test comment remove
#define SAUCE_COMMENT_REMOVE_ACTUAL_PATH    "actual/comment_remove_actual.txt"

// RemoveComment -> Contains the result of removing a comment from TestFile1.ans
#define SAUCE_REMOVECOMMENT_PATH            "expect/comment_remove/RemoveComment.ans"

// RemoveCommentAndAddEOF -> Contains the result of removing a comment from SauceButNoEOF.ans
#define SAUCE_REMOVECOMMENTANDADDEOF_PATH   "expect/comment_remove/RemoveCommentAndAddEOF.ans"


// The expected result of SAUCE_set_default
extern const SAUCE default_record;


SAUCE* test_get_testfile1_expected_record();

SAUCE_CommentBlock* test_get_testfile1_expected_comment();

SAUCE* test_get_testfile2_expected_record();

SAUCE* test_get_testfile3_expected_record();


// Test if a file's contents match another file's contents.
// Return true on a complete match, false if otherwise.
int test_file_matches_expected(const char* actual_filepath, const char* expected_filepath);

// Test if a buffer's contents match another file's contents.
// Return true on a complete match, false if otherwise.
int test_buffer_matches_expected(const char* buffer, uint32_t n, const char* expected_filepath);

// Copy a file from a src file to a dest file. Return 0 on success
int copy_file(const char* src, const char* dest);

// Copy the contents of a src file to a buffer. Return total bytes read on success.
int copy_file_into_buffer(const char* srcFile, char* buffer);

#endif //SAUCE_TEST_RES_HEADER_INCLUDED