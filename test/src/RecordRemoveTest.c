#include "unity.h"
#include "Sauce.h"
#include "TestRes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define REMOVE_BOTH_EXPECTED_LEN  24

// Global test buffer
char buffer[1024];

void setUp() {
  // clear the remove_actual file
  FILE* remove_actual = fopen(SAUCE_REMOVE_ACTUAL_PATH, "w");
  if (remove_actual == NULL) {
    fprintf(stderr, "Failed to open %s", SAUCE_REMOVE_ACTUAL_PATH);
    exit(1);
  }
  fclose(remove_actual);
}

void tearDown() {}





// Successful file remove test
void should_RemoveFromFile_when_FileContainsRecord() {
  // copy TestFile3 to remove_actual
  if (copy_file(SAUCE_TESTFILE3_PATH, SAUCE_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy TestFile3.ans to remove_actual.txt");
    return;  
  }

  int res = SAUCE_fremove(SAUCE_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_REMOVE_ACTUAL_PATH, SAUCE_REMOVE_ONLY_RECORD_PATH));
}


void should_RemoveFromFile_when_FileContainsCommentAndRecord() {
  // copy TestFile1 to remove_actual
  if (copy_file(SAUCE_TESTFILE1_PATH, SAUCE_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy TestFile1.ans to remove_actual.txt");
    return;  
  }

  int res = SAUCE_fremove(SAUCE_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_REMOVE_ACTUAL_PATH, SAUCE_REMOVE_RECORD_AND_COMMENT_PATH));
}


void should_RemoveFromFile_when_FileOnlyContainsRecord() {
  // copy TestFile2 to remove_actual
  if (copy_file(SAUCE_TESTFILE2_PATH, SAUCE_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy TestFile2.ans to remove_actual.txt");
    return;
  }

  int res = SAUCE_fremove(SAUCE_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(0, res);


  // attempt to read remove_actual
  FILE* actual = fopen(SAUCE_REMOVE_ACTUAL_PATH, "rb");
  if (actual == NULL) {
    TEST_FAIL_MESSAGE("Could not open remove_actual.txt");
    return;
  }

  char buffer[1];
  int read = fread(buffer, 1, 1, actual);
  fclose(actual);
  TEST_ASSERT_EQUAL(0, read);
}




// Successful buffer remove tests
void should_RemoveFromBuffer_when_BufferContainsRecord() {
  // copy TestFile3 into a buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE3_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile3.ans into a buffer");
    return;
  }

  int res = SAUCE_remove(buffer, length);
  TEST_ASSERT_EQUAL(length-129, res);
  TEST_ASSERT_TRUE(res > 0);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_REMOVE_ONLY_RECORD_PATH));
}


void should_RemoveFromBuffer_when_BufferContainsCommentAndRecord() {
  // copy TestFile1 into a buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans into a buffer");
    return;
  }

  int res = SAUCE_remove(buffer, length);
  TEST_ASSERT_EQUAL(REMOVE_BOTH_EXPECTED_LEN, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_REMOVE_RECORD_AND_COMMENT_PATH));
}


void should_RemoveFromBuffer_when_BufferOnlyContainsRecord() {
  // copy TestFile2 into a buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE2_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile2.ans into a buffer");
    return;
  }

  int res = SAUCE_remove(buffer, length);
  TEST_ASSERT_EQUAL(0, res);
}




// File remove failure tests

void should_FailToRemoveFromFile_when_FileDoesNotExist() {
  int res = SAUCE_fremove("expect/actual/MYFILEDNE.txt");
  TEST_ASSERT_EQUAL(SAUCE_EFOPEN, res);
}


void should_FailToRemoveFromFile_when_SAUCEIsMissing() {
  if (copy_file(SAUCE_NOSAUCE_PATH, SAUCE_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy NoSauce.txt to remove_actual.txt");
    return;
  }

  int res = SAUCE_fremove(SAUCE_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that remove_actual did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_REMOVE_ACTUAL_PATH, SAUCE_NOSAUCE_PATH));
}


void should_FailToRemoveFromFile_when_FileIsTooShort() {
  if (copy_file(SAUCE_SHORTFILE_PATH, SAUCE_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy ShortFile.txt to remove_actual.txt");
    return;
  }

  int res = SAUCE_fremove(SAUCE_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);

  // assert that remove_actual did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_REMOVE_ACTUAL_PATH, SAUCE_SHORTFILE_PATH));
}


void should_FailToRemoveFromFile_when_FilePathIsNULL() {
  int res = SAUCE_fremove(NULL);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToRemoveFromFile_when_FileHasNoEOFCharacter() {
  if (copy_file(SAUCE_SAUCEBUTNOEOF_PATH, SAUCE_REMOVE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy SauceButNoEOF.ans to remove_actual.txt");
    return;
  }

  int res = SAUCE_fremove(SAUCE_REMOVE_ACTUAL_PATH);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // asser that remove_actual did not change
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_REMOVE_ACTUAL_PATH, SAUCE_SAUCEBUTNOEOF_PATH));
}




// Buffer remove failure tests

void should_FailToRemoveFromBuf_when_SAUCEIsMissing() {
  int length = copy_file_into_buffer(SAUCE_NOSAUCE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Could not copy NoSauce.txt into a buffer");
    return;
  }

  int res = SAUCE_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that the buffer's contents have not been changed
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_NOSAUCE_PATH));
}


void should_FailToRemoveFromBuf_when_BufferIsTooShort() {
  int length = copy_file_into_buffer(SAUCE_SHORTFILE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Could not copy ShortFile.txt into a buffer");
    return;
  }

  int res = SAUCE_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);

  // assert that the buffer's contents have not been changed
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_SHORTFILE_PATH));
}


void should_FailToRemoveFromBuf_when_BufferIsNULL() {
  int res = SAUCE_remove(NULL, 200);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToRemoveFromBuf_when_BufferHasNoEOFCharacter() {  
  int length = copy_file_into_buffer(SAUCE_SAUCEBUTNOEOF_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Could not copy SauceButNoEOF.ans into a buffer");
    return;
  }

  int res = SAUCE_remove(buffer, length);
  TEST_ASSERT_EQUAL(SAUCE_ERMISS, res);

  // assert that the buffer's contents have not been changed
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_SAUCEBUTNOEOF_PATH));
}






int main(int argc, char** argv) {
  UNITY_BEGIN();

  RUN_TEST(should_RemoveFromFile_when_FileContainsRecord);
  RUN_TEST(should_RemoveFromFile_when_FileContainsCommentAndRecord);
  RUN_TEST(should_RemoveFromFile_when_FileOnlyContainsRecord);
  RUN_TEST(should_RemoveFromBuffer_when_BufferContainsRecord);
  RUN_TEST(should_RemoveFromBuffer_when_BufferContainsCommentAndRecord);
  RUN_TEST(should_RemoveFromBuffer_when_BufferOnlyContainsRecord);
  RUN_TEST(should_FailToRemoveFromFile_when_FileDoesNotExist);
  RUN_TEST(should_FailToRemoveFromFile_when_SAUCEIsMissing);
  RUN_TEST(should_FailToRemoveFromFile_when_FileIsTooShort);
  RUN_TEST(should_FailToRemoveFromFile_when_FilePathIsNULL);
  RUN_TEST(should_FailToRemoveFromFile_when_FileHasNoEOFCharacter);
  RUN_TEST(should_FailToRemoveFromBuf_when_SAUCEIsMissing);
  RUN_TEST(should_FailToRemoveFromBuf_when_BufferIsTooShort);
  RUN_TEST(should_FailToRemoveFromBuf_when_BufferIsNULL);
  RUN_TEST(should_FailToRemoveFromBuf_when_BufferHasNoEOFCharacter);

  return UNITY_END();
}





// TODO: for all other tests
// write -> if error, make sure the file did not change
// test for the situation where there is no EOF character but 128 bytes of a sauce record
// check if the filepath is NULL