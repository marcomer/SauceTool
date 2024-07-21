#include "unity.h"
#include "Sauce.h"
#include "TestRes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SHORT_COMMENT_MSG   "This is the short comment message. Simple, right!"



static char shortComment[SAUCE_COMMENT_LINE_LENGTH];
static char longComment[SAUCE_COMMENT_LINE_LENGTH * 25];
static char buffer[2048];


void setUp() {
  // create the short comment

  memset(shortComment, 0, SAUCE_COMMENT_LINE_LENGTH);
  memcpy(shortComment, SHORT_COMMENT_MSG, sizeof(SHORT_COMMENT_MSG) - 1);

  // create the long comment
  static char longComment[SAUCE_COMMENT_LINE_LENGTH * 25];
  memset(longComment, 0, SAUCE_COMMENT_LINE_LENGTH * 25);
  int len = copy_file_into_buffer(SAUCE_LONGNOSAUCE_PATH, longComment);
  if (len <= 0) {
    fprintf(stderr, "Failed to write %s to longComment string", SAUCE_LONGNOSAUCE_PATH);
    exit(-1);
  }

  // clear the buffer
  memset(buffer, 0, 2048);

  // clear comment_write_actual
  FILE* file = fopen(SAUCE_COMMENT_WRITE_ACTUAL_PATH, "w");
  if (file == NULL) {
    fprintf(stderr, "Could not open %s", SAUCE_COMMENT_WRITE_ACTUAL_PATH);
    exit(-1);
  }
  fclose(file);
}

void tearDown() {}




// File success cases

void should_AddComment_when_FileContainsRecord() {
  // Add shortComment to TestFile2.ans
  if (copy_file(SAUCE_TESTFILE2_PATH, SAUCE_COMMENT_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile2.ans to comment_write_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fwrite(SAUCE_COMMENT_WRITE_ACTUAL_PATH, shortComment);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_WRITE_ACTUAL_PATH, SAUCE_ADDCOMMENTTORECORD_PATH));
}


void should_ReplaceComment_when_FileContainsComment() {
  // Replace TestFile1.ans comment with longComment
  if (copy_file(SAUCE_TESTFILE1_PATH, SAUCE_COMMENT_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans to comment_write_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fwrite(SAUCE_COMMENT_WRITE_ACTUAL_PATH, longComment);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_WRITE_ACTUAL_PATH, SAUCE_REPLACEEXISTINGCOMMENT_PATH));
}


void should_AddCommentAndEOF_when_FileContainsRecordButNoEOF() {
  // Add shortComment to OnlyRecord.ans and add EOF character
  if (copy_file(SAUCE_ONLYRECORD_PATH, SAUCE_COMMENT_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy OnlyRecord.ans to comment_write_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fwrite(SAUCE_COMMENT_WRITE_ACTUAL_PATH, shortComment);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_WRITE_ACTUAL_PATH, SAUCE_ADDCOMMENTANDEOFTORECORD_PATH));
}


void should_ReplaceCommentAndAddEOF_when_FileContainsCommentButNoEOF() {
  // Replace comment in SauceButNoEOF.ans with smallComment and add EOF character
  if (copy_file(SAUCE_SAUCEBUTNOEOF_PATH, SAUCE_COMMENT_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy SauceButNoEOF to comment_write_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fwrite(SAUCE_COMMENT_WRITE_ACTUAL_PATH, shortComment);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_WRITE_ACTUAL_PATH, SAUCE_REPLACECOMMENTANDADDEOF_PATH));
}





// Buffer success cases

void should_AddComment_when_BufferContainsRecord() {
  // Add shortComment to TestFile2.ans buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE2_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile2.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_write(buffer, length, shortComment);
  TEST_ASSERT_EQUAL(198, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_ADDCOMMENTTORECORD_PATH));
}


void should_ReplaceComment_when_BufferContainsComment() {
  // Replace TestFile1.ans buffer comment with longComment
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_write(buffer, length, longComment);
  TEST_ASSERT_EQUAL(1758, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_REPLACEEXISTINGCOMMENT_PATH));
}


void should_AddCommentAndEOF_when_BufferContainsRecordButNoEOF() {
  // Add shortComment to OnlyRecord.ans buffer and add EOF character
  int length = copy_file_into_buffer(SAUCE_ONLYRECORD_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy OnlyRecord.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_write(buffer, length, shortComment);
  TEST_ASSERT_EQUAL(198, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_ADDCOMMENTANDEOFTORECORD_PATH));
}


void should_ReplaceCommentAndAddEOF_when_BufferContainsCommentButNoEOF() {
  // Replace comment in SauceButNoEOF.ans buffer with smallComment and add EOF character
  int length = copy_file_into_buffer(SAUCE_SAUCEBUTNOEOF_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy SauceButNoEOF.ans into the buffer");
    return;
  }

  int res = SAUCE_Comment_write(buffer, length, shortComment);
  TEST_ASSERT_EQUAL(222, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_REPLACECOMMENTANDADDEOF_PATH));
}





// File failure cases

void should_FailToWrite_when_FileDoesNotExist() {
  int res = SAUCE_Comment_fwrite("expect/FILEDOESNOTEXIST.mp4", shortComment);
  TEST_ASSERT_EQUAL(SAUCE_EFOPEN, res);
}


void should_FailToWrite_when_FileIsTooShort() {
  if (copy_file(SAUCE_SHORTFILE_PATH, SAUCE_COMMENT_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy ShortFile.txt to comment_write_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fwrite(SAUCE_COMMENT_WRITE_ACTUAL_PATH, shortComment);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);

  // assert that the file didn't change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_WRITE_ACTUAL_PATH, SAUCE_SHORTFILE_PATH));
}


void should_FailToWrite_when_FilePathIsNull() {
  int res = SAUCE_Comment_fwrite(NULL, shortComment);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToWriteToFile_when_CommentIsNull() {
  if (copy_file(SAUCE_TESTFILE1_PATH, SAUCE_COMMENT_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans to comment_write_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fwrite(SAUCE_COMMENT_WRITE_ACTUAL_PATH, NULL);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);

  // assert that the file didn't change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_WRITE_ACTUAL_PATH, SAUCE_TESTFILE1_PATH));
}


void should_FailToWriteToFile_when_FileContainsCommentButNoRecord() {
  if (copy_file(SAUCE_COMMENTBUTNORECORD_PATH, SAUCE_COMMENT_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to copy CommentButNoRecord.ans to comment_write_actual.txt");
    return;
  }

  int res = SAUCE_Comment_fwrite(SAUCE_COMMENT_WRITE_ACTUAL_PATH, shortComment);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that the file didn't change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_COMMENT_WRITE_ACTUAL_PATH, SAUCE_COMMENTBUTNORECORD_PATH));
}










// Buffer failure cases

void should_FailToWrite_when_BufferIsTooShort() {
  int length = copy_file_into_buffer(SAUCE_SHORTFILE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans to the buffer");
    return;
  }

  int res = SAUCE_Comment_write(buffer, length, shortComment);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);

  // assert that the buffer has not changed
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_SHORTFILE_PATH));
}


void should_FailToWRite_when_BufferIsNull() {
  int res = SAUCE_Comment_write(NULL, 256, shortComment);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToWriteToBuffer_when_CommentIsNull() {
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length < 0) {
    TEST_FAIL_MESSAGE("Failed to copy CommentButNoRecord.ans to the buffer");
    return;
  }

  int res = SAUCE_Comment_write(buffer, length, NULL);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);

  // assert that the buffer didn't change
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_TESTFILE1_PATH));
}



void should_FailToWriteToBuffer_when_BufferContainsCommentButNoRecord() {
  int length = copy_file_into_buffer(SAUCE_COMMENTBUTNORECORD_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy CommentButNoRecord.ans to the buffer");
    return;
  }

  int res = SAUCE_Comment_write(buffer, length, shortComment);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);
  
  // assert that the buffer hasn't changed
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_COMMENTBUTNORECORD_PATH));
}






int main(int argc, char** argv) {
  UNITY_BEGIN();

  RUN_TEST(should_AddComment_when_FileContainsRecord);
  RUN_TEST(should_ReplaceComment_when_FileContainsComment);
  RUN_TEST(should_AddCommentAndEOF_when_FileContainsRecordButNoEOF);
  RUN_TEST(should_ReplaceCommentAndAddEOF_when_FileContainsCommentButNoEOF);
  RUN_TEST(should_AddComment_when_BufferContainsRecord);
  RUN_TEST(should_ReplaceComment_when_BufferContainsComment);
  RUN_TEST(should_AddCommentAndEOF_when_BufferContainsRecordButNoEOF);
  RUN_TEST(should_ReplaceCommentAndAddEOF_when_BufferContainsCommentButNoEOF);
  RUN_TEST(should_FailToWrite_when_FileDoesNotExist);
  RUN_TEST(should_FailToWrite_when_FileIsTooShort);
  RUN_TEST(should_FailToWrite_when_FilePathIsNull);
  RUN_TEST(should_FailToWriteToFile_when_CommentIsNull);
  RUN_TEST(should_FailToWriteToFile_when_FileContainsCommentButNoRecord);
  RUN_TEST(should_FailToWrite_when_BufferIsTooShort);
  RUN_TEST(should_FailToWRite_when_BufferIsNull);
  RUN_TEST(should_FailToWriteToBuffer_when_CommentIsNull);
  RUN_TEST(should_FailToWriteToBuffer_when_BufferContainsCommentButNoRecord);

  return UNITY_END();
}