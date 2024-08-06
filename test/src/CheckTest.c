#include "unity.h"
#include "SauceTool.h"
#include "TestRes.h"
#include <string.h>
#include <stdio.h>



static char buffer[2048];

void setUp() {
  // clear the buffer
  memset(buffer, 0, 2048);
}

void tearDown() {}






// Check file success

void should_PassCheck_when_FileContainsRecordAndComment() {
  // Check TestFile1.ans
  TEST_ASSERT_TRUE(SAUCE_check_file(SAUCE_TESTFILE1_PATH));
}


void should_PassCheck_when_FileDoesNotContainEOF() {
  // check SauceButNoEOF.ans
  TEST_ASSERT_TRUE(SAUCE_check_file(SAUCE_SAUCEBUTNOEOF_PATH));
}


void should_PassCheck_when_FileOnlyContainsRecord() {
  // check OnlyRecord.ans
  TEST_ASSERT_TRUE(SAUCE_check_file(SAUCE_ONLYRECORD_PATH));
}






// Check buffer success

void should_PassCheck_when_BufferContainsRecordAndComment() {
  // check TestFile1 buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans into the buffer");
    return;
  }

  TEST_ASSERT_TRUE(SAUCE_check_buffer(buffer, length));
}


void should_PassCheck_when_BufferDoesNotContainEOF() {
  // check SauceButNoEOF buffer
  int length = copy_file_into_buffer(SAUCE_SAUCEBUTNOEOF_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy SauceButNoEOF.ans into the buffer");
    return;
  }

  TEST_ASSERT_TRUE(SAUCE_check_buffer(buffer, length));
}


void should_PassCheck_when_BufferOnlyContainsRecord() {
  // check OnlyRecord buffer
    int length = copy_file_into_buffer(SAUCE_ONLYRECORD_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy OnlyRecord.ans into the buffer");
    return;
  }

  TEST_ASSERT_TRUE(SAUCE_check_buffer(buffer, length));
}





// Check file failure

void should_FailCheck_when_FileDoesNotExist() {
  TEST_ASSERT_FALSE(SAUCE_check_file("expect/thisfiledoesnotexist.js"));
}


void should_FailCheck_when_FilePathIsNull() {
  TEST_ASSERT_FALSE(SAUCE_check_file(NULL));
}


void should_FailCheck_when_FileIsTooShort() {
  TEST_ASSERT_FALSE(SAUCE_check_file(SAUCE_SHORTFILE_PATH));
}


void should_FailCheck_when_FileIsEmpty() {
  TEST_ASSERT_FALSE(SAUCE_check_file(SAUCE_EMPTYFILE_PATH));
}


void should_FailCheckOnFile_when_CommentIsInvalid() {
  TEST_ASSERT_FALSE(SAUCE_check_file(SAUCE_INVALIDCOMMENT_PATH));
}


void should_FailCheck_when_FileHasNoRecord() {
  TEST_ASSERT_FALSE(SAUCE_check_file(SAUCE_NOSAUCE_PATH));
}


void should_FailCheck_when_FileHasCommentButNoRecord() {
  TEST_ASSERT_FALSE(SAUCE_check_file(SAUCE_COMMENTBUTNORECORD_PATH));
}





// Check buffer failure

void should_FailCheck_when_BufferIsNull() {
  TEST_ASSERT_FALSE(SAUCE_check_buffer(NULL, 256));
}


void should_FailCheck_when_BufferIsTooShort() {
  int length = copy_file_into_buffer(SAUCE_SHORTFILE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy ShortFile.txt into the buffer");
    return;
  }

  TEST_ASSERT_FALSE(SAUCE_check_buffer(buffer, length));
}


void should_FailCheck_when_BufferIsEmpty() {
  TEST_ASSERT_FALSE(SAUCE_check_buffer(buffer, 0));
}


void should_FailCheckOnBuffer_when_CommentIsInvalid() {
  int length = copy_file_into_buffer(SAUCE_INVALIDCOMMENT_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy InvalidComment.txt into the buffer");
    return;
  }

  TEST_ASSERT_FALSE(SAUCE_check_buffer(buffer, length));
}


void should_FailCheck_when_BufferHasNoRecord() {
  int length = copy_file_into_buffer(SAUCE_NOSAUCE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy NoSauce.txt into the buffer");
    return;
  }

  TEST_ASSERT_FALSE(SAUCE_check_buffer(buffer, length));
}


void should_FailCheck_when_BufferHasCommentButNoRecord() {
  int length = copy_file_into_buffer(SAUCE_COMMENTBUTNORECORD_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy CommentButNoRecord.ans into the buffer");
    return;
  }

  TEST_ASSERT_FALSE(SAUCE_check_buffer(buffer, length));
}






int main(int argc, char** argv) {
  UNITY_BEGIN();

  RUN_TEST(should_PassCheck_when_FileContainsRecordAndComment);
  RUN_TEST(should_PassCheck_when_FileDoesNotContainEOF);
  RUN_TEST(should_PassCheck_when_FileOnlyContainsRecord);
  RUN_TEST(should_PassCheck_when_BufferContainsRecordAndComment);
  RUN_TEST(should_PassCheck_when_BufferDoesNotContainEOF);
  RUN_TEST(should_PassCheck_when_BufferOnlyContainsRecord);
  RUN_TEST(should_FailCheck_when_FileDoesNotExist);
  RUN_TEST(should_FailCheck_when_FilePathIsNull);
  RUN_TEST(should_FailCheck_when_FileIsTooShort);
  RUN_TEST(should_FailCheck_when_FileIsEmpty);
  RUN_TEST(should_FailCheckOnFile_when_CommentIsInvalid);
  RUN_TEST(should_FailCheck_when_FileHasNoRecord);
  RUN_TEST(should_FailCheck_when_FileHasCommentButNoRecord);
  RUN_TEST(should_FailCheck_when_BufferIsNull);
  RUN_TEST(should_FailCheck_when_BufferIsTooShort);
  RUN_TEST(should_FailCheck_when_BufferIsEmpty);
  RUN_TEST(should_FailCheckOnBuffer_when_CommentIsInvalid);
  RUN_TEST(should_FailCheck_when_BufferHasNoRecord);
  RUN_TEST(should_FailCheck_when_BufferHasCommentButNoRecord);

  SAUCE_clear_error();
  return UNITY_END();
}