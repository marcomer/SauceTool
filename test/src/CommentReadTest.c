#include "unity.h"
#include "Sauce.h"
#include "TestRes.h"
#include <string.h>
#include <stdio.h>



static SAUCE_CommentBlock block;
static char blockStr[UINT8_MAX * SAUCE_COMMENT_LINE_LENGTH];
static char buffer[1024];



void setUp() {
  // clear the buffer
  memset(buffer, 0, 1024);

  // reset the CommentBlock struct
  memset(blockStr, 0, UINT8_MAX * SAUCE_COMMENT_LINE_LENGTH);
  memcpy(block.ID, "COMNT", 5);
  block.lines = 0;
  block.comment = blockStr;
}


void tearDown() {}






// File success cases

void should_ReadComment_when_FileContainsComment() {
  // read TestFile1.ans
  int res = SAUCE_Comment_fread(SAUCE_TESTFILE1_PATH, &block, 2);
  TEST_ASSERT_EQUAL(2, res);

  TEST_ASSERT_TRUE(SAUCE_Comment_equal(&block, test_get_testfile1_expected_comment()));
}


void should_ReadNothing_when_FileContainsNoComment() {
  // read NoSauce.txt
  int res = SAUCE_Comment_fread(SAUCE_NOSAUCE_PATH, &block, 2);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_EQUAL_CHAR_ARRAY("COMNT", block.ID, 5);
  TEST_ASSERT_EQUAL(0, block.lines);
}


void should_ReadFullCommentFromFile_when_MoreLinesRequestedThanAvailable() {
  // read TestFile1.ans, but request 3 liens
  int res = SAUCE_Comment_fread(SAUCE_TESTFILE1_PATH, &block, 3);
  TEST_ASSERT_EQUAL(2, res);

  TEST_ASSERT_TRUE(SAUCE_Comment_equal(&block, test_get_testfile1_expected_comment()));
}




// Buffer success cases

void should_ReadComment_when_BufferContainsComment() {
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans to the buffer");
    return;
  }

  int res = SAUCE_Comment_read(buffer, length, &block, 2);
  TEST_ASSERT_EQUAL(2, res);

  TEST_ASSERT_TRUE(SAUCE_Comment_equal(&block, test_get_testfile1_expected_comment()));
}


void should_ReadNothing_when_BufferContainsNoComment() {
  int length = copy_file_into_buffer(SAUCE_NOSAUCE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy NoSauce.txt to the buffer");
    return;
  }

  int res = SAUCE_Comment_read(buffer, length, &block, 2);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_EQUAL_CHAR_ARRAY("COMNT", block.ID, 5);
  TEST_ASSERT_EQUAL(0, block.lines);
}


void should_ReadFullCommentFromBuffer_when_MoreLinesRequestedThanAvailable() {
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans to the buffer");
    return;
  }

  int res = SAUCE_Comment_read(buffer, length, &block, 3);
  TEST_ASSERT_EQUAL(2, res);

  TEST_ASSERT_TRUE(SAUCE_Comment_equal(&block, test_get_testfile1_expected_comment()));
}





// File failure cases

void should_FailToRead_when_FileDoesNotExist() {
  int res = SAUCE_Comment_fread("expect/NOFILELIKETHISEXISTS.txt", &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_EFOPEN, res);
}


void should_FailToRead_when_FileHasCommentButNoRecord() {
  int res = SAUCE_Comment_fread(SAUCE_COMMENTBUTNORECORD_PATH, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);
}


void should_FailToRead_when_FileHasNoSauce() {
  int res = SAUCE_Comment_fread(SAUCE_NOSAUCE_PATH, &block, 1);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);
}


void should_FailToRead_when_FileIsTooShort() {
  int res = SAUCE_Comment_fread(SAUCE_SHORTFILE_PATH, &block, 1);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);
}


void should_FailToReadFromFile_when_CommentIsInvalid() {
  // Comment is missing the COMNT id.
  // Meaning that the Comments field is incorrect.

  int res = SAUCE_Comment_fread(SAUCE_INVALIDCOMMENT_PATH, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ECMISS, res);
}


void should_FailToRead_when_FileSaucePointerIsNull() {
  int res = SAUCE_Comment_fread(SAUCE_TESTFILE1_PATH, NULL, 2);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToRead_when_FilePathIsNull() {
  int res = SAUCE_Comment_fread(NULL, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToReadFromFile_when_CommentStringIsNull() {
  block.comment = NULL;
  int res = SAUCE_Comment_fread(SAUCE_TESTFILE1_PATH, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}





// Buffer failure cases

void should_FailToRead_when_BufferHasCommentButNoRecord() {
  int length = copy_file_into_buffer(SAUCE_COMMENTBUTNORECORD_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy CommentButNoRecord.ans to the buffer");
    return;
  }

  int res = SAUCE_Comment_read(buffer, length, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);
}


void should_FailToRead_when_BufferHasNoSAUCE() {
  int length = copy_file_into_buffer(SAUCE_NOSAUCE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy NoSauce.txt to the buffer");
    return;
  }

  int res = SAUCE_Comment_read(buffer, length, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);
}


void should_FailToReadFromBuffer_when_CommentIsInvalid() {
  // Comment is missing the COMNT id.
  // Meaning that the Comments field is incorrect.
  int length = copy_file_into_buffer(SAUCE_INVALIDCOMMENT_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy InvalidComment.txt to the buffer");
    return;
  }

  int res = SAUCE_Comment_read(buffer, length, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ECMISS, res);
}


void should_FailToRead_when_BufferSaucePointerIsNull() {
  int res = SAUCE_Comment_read(buffer, 256, NULL, 2);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToRead_when_BufferIsNull() {
  int res = SAUCE_Comment_read(NULL, 256, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToRead_when_BufferLengthIsTooShort() {
  int length = copy_file_into_buffer(SAUCE_SHORTFILE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy ShortFile.txt to the buffer");
    return;
  }

  int res = SAUCE_Comment_read(buffer, length, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);
}


void should_FailToReadFromBuffer_when_CommentStringIsNull() {
  block.comment = NULL;
  int res = SAUCE_Comment_read(buffer, 256, &block, 2);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}





int main(int argc, char** argv) {
  UNITY_BEGIN();

  RUN_TEST(should_ReadComment_when_FileContainsComment);
  RUN_TEST(should_ReadNothing_when_FileContainsNoComment);
  RUN_TEST(should_ReadFullCommentFromFile_when_MoreLinesRequestedThanAvailable);
  RUN_TEST(should_ReadComment_when_BufferContainsComment);
  RUN_TEST(should_ReadNothing_when_BufferContainsNoComment);
  RUN_TEST(should_ReadFullCommentFromBuffer_when_MoreLinesRequestedThanAvailable);
  RUN_TEST(should_FailToRead_when_FileDoesNotExist);
  RUN_TEST(should_FailToRead_when_FileHasCommentButNoRecord);
  RUN_TEST(should_FailToRead_when_FileHasNoSauce);
  RUN_TEST(should_FailToRead_when_FileIsTooShort);
  RUN_TEST(should_FailToReadFromFile_when_CommentIsInvalid);
  RUN_TEST(should_FailToRead_when_FileSaucePointerIsNull);
  RUN_TEST(should_FailToRead_when_FilePathIsNull);
  RUN_TEST(should_FailToReadFromFile_when_CommentStringIsNull);
  RUN_TEST(should_FailToRead_when_BufferHasCommentButNoRecord);
  RUN_TEST(should_FailToRead_when_BufferHasNoSAUCE);
  RUN_TEST(should_FailToReadFromBuffer_when_CommentIsInvalid);
  RUN_TEST(should_FailToRead_when_BufferSaucePointerIsNull);
  RUN_TEST(should_FailToRead_when_BufferIsNull);
  RUN_TEST(should_FailToRead_when_BufferLengthIsTooShort);
  RUN_TEST(should_FailToReadFromBuffer_when_CommentStringIsNull);

  return UNITY_END();
}
