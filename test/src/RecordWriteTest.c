#include "unity.h"
#include "Sauce.h"
#include "TestRes.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// RecordWriteTest, tests all SAUCE record write functions


void set_sauce(SAUCE* sauce) {
  SAUCE_set_default(sauce);

  memcpy(sauce->Title, "WriteFile", 9);
  memcpy(sauce->Author, "testauthor", 10);
  memcpy(sauce->Group, "NoGroup", 7);
  memcpy(sauce->Date, "20000101", 8);
  memcpy(sauce->TInfoS, "FontName", 8);

  sauce->DataType = 2;
  sauce->FileType = 1;
  sauce->TInfo1 = 99;
  sauce->TInfo2 = 45;
  sauce->TInfo3 = 129;
  sauce->TInfo4 = UINT16_MAX;
  sauce->Comments = 1;
  sauce->TFlags = 0x02;
}



static SAUCE sauce;
static char buffer[1024];

void setUp() {
  set_sauce(&sauce);

  // clear the write_actual file
  FILE* write_actual = fopen(SAUCE_WRITE_ACTUAL_PATH, "w");
  if (write_actual == NULL) {
    fprintf(stderr, "Failed to open %s", SAUCE_WRITE_ACTUAL_PATH);
    exit(1);
  }
  fclose(write_actual);

  // clear the global buffer
  memset(buffer, 0, 1024);
}

void tearDown() {
}




// File success cases

void should_WriteToFile_when_FileIsEmpty() {
  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, &sauce);
  TEST_ASSERT_EQUAL_INT(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_WRITETOEMPTY_PATH));
}


void should_AppendToFile_when_FileContainsContent() {
  // copy NoSauce.txt to actual
  if (copy_file(SAUCE_NOSAUCE_PATH, SAUCE_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy NoSauce.txt to write_actual.txt");
    return;
  }

  // append SAUCE to actual
  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, &sauce);
  TEST_ASSERT_EQUAL_INT(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_APPEND_PATH));
}


void should_AppendToFileAndAddEOF_when_FileContainsContentAndEOF() {
  // copy NoSauceWithEOF.ans to actual
  // an EOF should be added and the original EOF should also remain
  if (copy_file(SAUCE_NOSAUCEWITHEOF_PATH, SAUCE_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy NoSauceWithEOF.ans to write_actual.txt");
    return;
  }

  // append SAUCE to actual
  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, &sauce);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_APPENDWITHEOF_PATH));
}


void should_ReplaceSAUCE_when_FileContainsSAUCE() {
  // copy TestFile1.ans to actual
  if (copy_file(SAUCE_TESTFILE1_PATH, SAUCE_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy TestFile1.txt to write_actual.txt");
    return;
  }

  // replace actual's SAUCE
  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, &sauce);
  TEST_ASSERT_EQUAL_INT(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_REPLACE_PATH));
}


void should_ReplaceSAUCE_when_FileOnlyContainsSAUCE() {
  // copy TestFile2.ans to actual
  if (copy_file(SAUCE_TESTFILE2_PATH, SAUCE_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy TestFile2.ans to write_actual.txt");
    return;
  }

  // replace actual's SAUCE
  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, &sauce);
  TEST_ASSERT_EQUAL(0, res);

  // WriteToEmpty is equivalent to replacing a file that only contains SAUCE
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_WRITETOEMPTY_PATH));
}


void should_ReplaceSAUCEAndAddEOF_when_FileContainsFullSAUCEWithNoEOF() {
  // copy SauceButNoEOF.ans to actual
  if (copy_file(SAUCE_SAUCEBUTNOEOF_PATH, SAUCE_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy SauceButNoEOF.ans to write_actual.txt");
    return;
  }

  // replace actual's SAUCE and add EOF
  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, &sauce);
  TEST_ASSERT_EQUAL(0, res);

  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_REPLACE_PATH));
}


void should_ReplaceSAUCEAndAddEOF_when_FileOnlyContainsRecordWithNoEOF() {
  // copy OnlyRecord.ans to actual
  if (copy_file(SAUCE_ONLYRECORD_PATH, SAUCE_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Could not copy OnlyRecord.ans to write_actual.txt");
    return;
  }

  // replace actual's SAUCE and add EOF
  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, &sauce);
  TEST_ASSERT_EQUAL(0, res);

  // replacing record in OnlyRecord.ans and adding EOF is equivalent to WriteToEmptyExpect
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_WRITETOEMPTY_PATH));
}







// Buffer success cases

void should_WriteToBuffer_when_BufferLengthIsZero() {
  int res = SAUCE_write(buffer, 0, &sauce);
  TEST_ASSERT_EQUAL_INT(129, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, 129, SAUCE_WRITETOEMPTY_PATH));
}


void should_AppendToBuffer_when_BufferContainsContent() {
  // copy NoSauce.txt into buffer
  int length = copy_file_into_buffer(SAUCE_NOSAUCE_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy NoSauce.txt into the buffer");
    return;
  }
  
  // append to buffer
  int res = SAUCE_write(buffer, length, &sauce);
  TEST_ASSERT_EQUAL(length + 129, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length + 129, SAUCE_APPEND_PATH));
}


void should_AppendToBufferAndAddEOF_when_BufferContainsContentAndEOF() {
  // copy NoSauceWithEOF.ans to actual
  // an EOF should be added and the original EOF should also remain
  int length = copy_file_into_buffer(SAUCE_NOSAUCEWITHEOF_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy NoSauceWithEOF.ans into the buffer");
    return;
  }

  // append SAUCE to actual
  int res = SAUCE_write(buffer, length, &sauce);
  TEST_ASSERT_EQUAL(length + 129, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length + 129, SAUCE_APPENDWITHEOF_PATH));
}


void should_ReplaceSAUCE_when_BufferContainsSAUCE() {
  // copy TestFile1.ans into buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans into the buffer");
    return;
  }

  // replace buffer's sauce
  int res = SAUCE_write(buffer, length, &sauce);
  TEST_ASSERT_EQUAL(length, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_REPLACE_PATH));
}


void should_ReplaceSAUCE_when_BufferOnlyContainsSAUCE() {
  // copy TestFile2.ans to that buffer
  int length = copy_file_into_buffer(SAUCE_TESTFILE2_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile2.ans into the buffer");
    return;
  }

  // replace buffer's SAUCE
  int res = SAUCE_write(buffer, length, &sauce);
  TEST_ASSERT_EQUAL(length, res);

  // WriteToEmpty is equivalent to replacing a file that only contains SAUCE
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_WRITETOEMPTY_PATH));
}


void should_ReplaceSAUCEAndAddEOF_when_BufferContainsFullSAUCEWithNoEOF() {
  // copy SauceButNoEOF.ans to the buffer
  int length = copy_file_into_buffer(SAUCE_SAUCEBUTNOEOF_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy SauceButNoEOF.ans into the buffer");
    return;
  }

  // replace buffer's SAUCE and add EOF
  int res = SAUCE_write(buffer, length, &sauce);
  TEST_ASSERT_EQUAL(length + 1, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_REPLACE_PATH));
}


void should_ReplaceSAUCEAndAddEOF_when_BufferOnlyContainsRecordWithNoEOF() {
  // copy OnlyRecord.ans to actual
  int length = copy_file_into_buffer(SAUCE_ONLYRECORD_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy OnlyRecord.ans into the buffer");
    return;
  }

  // replace buffer's SAUCE and add EOF
  int res = SAUCE_write(buffer, length, &sauce);
  TEST_ASSERT_EQUAL(length + 1, res);

  // replacing record in OnlyRecord.ans and adding EOF is equivalent to WriteToEmptyExpect
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, res, SAUCE_WRITETOEMPTY_PATH));
}




// File fail cases
void should_FailToWrite_when_FileDoesNotExist() {
  int res = SAUCE_fwrite("expect/weird/File123456.txt", &sauce);
  TEST_ASSERT_EQUAL(SAUCE_EFOPEN, res);
}


void should_FailToWrite_when_FilePathIsNull() {
  int res = SAUCE_fwrite(NULL, &sauce);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToWrite_when_FileSauceIsNull() {
  if (copy_file(SAUCE_TESTFILE1_PATH, SAUCE_WRITE_ACTUAL_PATH) != 0) {
    TEST_FAIL_MESSAGE("Failed to write TestFile1.ans to write_actual.txt");
    return;
  }

  int res = SAUCE_fwrite(SAUCE_WRITE_ACTUAL_PATH, NULL);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);

  // assert that write_actual has not changed
  TEST_ASSERT_TRUE(test_file_matches_expected(SAUCE_WRITE_ACTUAL_PATH, SAUCE_TESTFILE1_PATH));
}




// Buffer fail cases
void should_FailToWrite_when_BufferIsNull() {
  int res = SAUCE_write(NULL, 256, &sauce);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailToWrite_when_BufferSauceIsNull() {
  int length = copy_file_into_buffer(SAUCE_TESTFILE1_PATH, buffer);
  if (length <= 0) {
    TEST_FAIL_MESSAGE("Failed to copy TestFile1.ans into the buffer");
    return;
  }

  int res = SAUCE_write(buffer, length, NULL);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);

  // assert that the buffer has not changed
  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, length, SAUCE_TESTFILE1_PATH));
}





int main(int argc, char** argv) {
  UNITY_BEGIN();
  
  RUN_TEST(should_WriteToFile_when_FileIsEmpty);
  RUN_TEST(should_AppendToFile_when_FileContainsContent);
  RUN_TEST(should_AppendToFileAndAddEOF_when_FileContainsContentAndEOF);
  RUN_TEST(should_ReplaceSAUCE_when_FileContainsSAUCE);
  RUN_TEST(should_ReplaceSAUCE_when_FileOnlyContainsSAUCE);
  RUN_TEST(should_ReplaceSAUCEAndAddEOF_when_FileContainsFullSAUCEWithNoEOF);
  RUN_TEST(should_ReplaceSAUCEAndAddEOF_when_FileOnlyContainsRecordWithNoEOF);
  RUN_TEST(should_WriteToBuffer_when_BufferLengthIsZero);
  RUN_TEST(should_AppendToBuffer_when_BufferContainsContent);
  RUN_TEST(should_AppendToBufferAndAddEOF_when_BufferContainsContentAndEOF);
  RUN_TEST(should_ReplaceSAUCE_when_BufferContainsSAUCE);
  RUN_TEST(should_ReplaceSAUCE_when_BufferOnlyContainsSAUCE);
  RUN_TEST(should_ReplaceSAUCEAndAddEOF_when_BufferContainsFullSAUCEWithNoEOF);
  RUN_TEST(should_ReplaceSAUCEAndAddEOF_when_BufferOnlyContainsRecordWithNoEOF);
  RUN_TEST(should_FailToWrite_when_FileDoesNotExist);
  RUN_TEST(should_FailToWrite_when_FilePathIsNull);
  RUN_TEST(should_FailToWrite_when_FileSauceIsNull);
  RUN_TEST(should_FailToWrite_when_BufferIsNull);
  RUN_TEST(should_FailToWrite_when_BufferSauceIsNull);

  return UNITY_END();
}
