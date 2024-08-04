#include "unity.h"
#include "SauceTool.h"
#include "TestRes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



static char buffer[2048];

void setUp() {
  // clear the buffer
  memset(buffer, 0, 2048);

  // clear comment_remove_actual file
  FILE* file = fopen(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, "w");
  if (file == NULL) {
    fprintf(stderr, "Could not open %s", SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
    exit(-1);
  }
  fclose(file);
}

void tearDown() {}





// File success cases

void should_RemoveComment_when_FileContainsComment() {
  // remove comment from TestFile1.ans
  if (copy_file(SAUCE_TESTFILE1_PATH, SAUCE_COMMENT_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans to comment_remove_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_REMOVECOMMENT_PATH));
}


void should_RemoveCommentAndAddEOF_when_FileContainsCommentButNoEOF() {
  // remove comment from SauceButNoEOF.ans
  if (copy_file(SAUCE_SAUCEBUTNOEOF_PATH, SAUCE_COMMENT_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy SauceButNoEOF.ans to comment_remove_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_REMOVECOMMENTANDADDEOF_PATH));
}





// Buffer success cases

void should_RemoveComment_when_BufferContainsComment() {
  // remove comment from TestFile1.ans buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, length);
  TEST_ASSERT_EQUAL(153, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_REMOVECOMMENT_PATH));
}


void should_RemoveCommentAndAddEOF_when_BufferContainsCommentButNoEOF() {
  // remove comment from SauceButNoEOF.ans buffer
  int length = copy_file_into_buffer(SAUCE_SAUCEBUTNOEOF_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy SauceButNoEOF.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, length);
  TEST_ASSERT_EQUAL(153, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_REMOVECOMMENTANDADDEOF_PATH));
}





// File failure cases

void should_FailToRemove_when_FileDoesNotExist() {
  int res = SAUCE_Comment_fremove("expect/thisdoesntexistman.ans");
  TEST_ASSERT_EQUAL(SAUCE_EFOPEN, res);
}


void should_FailToRemove_when_FileIsTooShort() {
  if (copy_file(SAUCE_SHORTFILE_PATH, SAUCE_COMMENT_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy ShortFile.txt to comment_remove_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);

  // assert that the file did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_SHORTFILE_PATH));
}


void should_FailToRemove_when_FileIsEmpty() {
  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_EEMPTY, res);

  // assert that the file did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_EMPTYFILE_PATH));
}


void should_FailToRemove_when_FilePathIsNull() {
  int res = SAUCE_Comment_fremove(NULL);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToRemoveFromFile_when_CommentIsInvalid() {
  // Comment is invalid if the COMNT id was not in the right place,
  // meaning that the Comments field in the record is likely wrong.
  if (copy_file(SAUCE_INVALIDCOMMENT_PATH, SAUCE_COMMENT_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy InvalidComment.txt to comment_remmove_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ECMISS, res);

  // assert that the file did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_INVALIDCOMMENT_PATH));
}


void should_FailToRemove_when_FileContainsNoComment() {
  if (copy_file(SAUCE_TESTFILE2_PATH, SAUCE_COMMENT_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile2.ans to comment_remove_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ECMISS, res);

  // assert that the file did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_TESTFILE2_PATH));
}


void should_FailToRemove_when_FileContainsNoRecord() {
  if (copy_file(SAUCE_NOSAUCE_PATH, SAUCE_COMMENT_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy NoSauce.txt to comment_remove_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that the file did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_NOSAUCE_PATH));
}


void should_FailToRemove_when_FileHasCommentButNoRecord() {
  if (copy_file(SAUCE_COMMENTBUTNORECORD_PATH, SAUCE_COMMENT_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy CommentbutNoRecord.ans to comment_remove_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fremove(SAUCE_COMMENT_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that the file did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_REMOVE_ACTUAL_PATH, SAUCE_COMMENTBUTNORECORD_PATH));
}




// Buffer failure cases

void should_FailToRemove_when_BufferIsTooShort() {
  int length = copy_file_into_buffer(SAUCE_SHORTFILE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy ShortFile.txt into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);

  // assert that the buffer didn't change
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_SHORTFILE_PATH));
}


void should_FailToRemove_when_BufferIsEmpty() {
  int length = copy_file_into_buffer(SAUCE_SHORTFILE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy ShortFile.txt into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, 0);
  TEST_ASSERT_EQUAL(SAUCE_EEMPTY, res);

  // assert that the buffer didn't change
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_SHORTFILE_PATH));
}


void should_FailToRemove_when_BufferIsNull() {
  int res = SAUCE_Comment_remove(NULL, 256);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToRemoveFromBuffer_when_CommentIsInvalid() {
  int length = copy_file_into_buffer(SAUCE_INVALIDCOMMENT_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy InvalidComment.txt into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ECMISS, res);

  // assert that the buffer didn't change
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_INVALIDCOMMENT_PATH));
}


void should_FailToRemove_when_BufferContainsNoComment() {
  int length = copy_file_into_buffer(SAUCE_TESTFILE2_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile2.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ECMISS, res);

  // assert that the buffer didn't change
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_TESTFILE2_PATH));
}


void should_FailToRemove_when_BufferContainsNoRecord() {
  int length = copy_file_into_buffer(SAUCE_NOSAUCE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy NoSauce.txt into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that the buffer didn't change
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_NOSAUCE_PATH));
}


void should_FailToRemove_when_BufferHasCommentButNoRecord() {
  int length = copy_file_into_buffer(SAUCE_COMMENTBUTNORECORD_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy CommentButNoRecord.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that the buffer didn't change
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_COMMENTBUTNORECORD_PATH));
}






int main(int argc, char** argv) {
  UNITY_BEGIN();

  RUN_TEST(should_RemoveComment_when_FileContainsComment);
  RUN_TEST(should_RemoveCommentAndAddEOF_when_FileContainsCommentButNoEOF);
  RUN_TEST(should_RemoveComment_when_BufferContainsComment);
  RUN_TEST(should_RemoveCommentAndAddEOF_when_BufferContainsCommentButNoEOF);
  RUN_TEST(should_FailToRemove_when_FileDoesNotExist);
  RUN_TEST(should_FailToRemove_when_FileIsTooShort);
  RUN_TEST(should_FailToRemove_when_FileIsEmpty);
  RUN_TEST(should_FailToRemove_when_FilePathIsNull);
  RUN_TEST(should_FailToRemoveFromFile_when_CommentIsInvalid);
  RUN_TEST(should_FailToRemove_when_FileContainsNoComment);
  RUN_TEST(should_FailToRemove_when_FileContainsNoRecord);
  RUN_TEST(should_FailToRemove_when_FileHasCommentButNoRecord);
  RUN_TEST(should_FailToRemove_when_BufferIsTooShort);
  RUN_TEST(should_FailToRemove_when_BufferIsEmpty);
  RUN_TEST(should_FailToRemove_when_BufferIsNull);
  RUN_TEST(should_FailToRemoveFromBuffer_when_CommentIsInvalid);
  RUN_TEST(should_FailToRemove_when_BufferContainsNoComment);
  RUN_TEST(should_FailToRemove_when_BufferContainsNoRecord);
  RUN_TEST(should_FailToRemove_when_BufferHasCommentButNoRecord);

  return UNITY_END();
}