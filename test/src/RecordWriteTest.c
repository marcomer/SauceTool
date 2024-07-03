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
  memcpy(sauce->Group, "NoGroup", 8);
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

void setUp() {
  set_sauce(&sauce);

  // clear the write_actual file
  FILE* write_actual = fopen("expect/actual/write_actual.txt", "w");
  if (write_actual == NULL) {
    fprintf(stderr, "Failed to open expect/actual/write_actual.txt");
    exit(1);
  }
  fclose(write_actual);
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




// Buffer success cases

void should_WriteToBuffer_when_BufferLengthIsZero() {
  char buffer[256];
  memset(buffer, 0, 256);

  int res = SAUCE_write(buffer, 0, &sauce);
  TEST_ASSERT_EQUAL_INT(129, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, 129, SAUCE_WRITETOEMPTY_PATH));
}


void should_AppendToBuffer_when_BufferContainsContent() {
  // copy NoSauce.txt into buffer
  char buffer[1024];
  FILE* file = fopen(SAUCE_NOSAUCE_PATH, "rb");
  if (file == NULL) {
    TEST_FAIL_MESSAGE("Could not open "SAUCE_NOSAUCE_PATH);
    return;
  }

  int read;
  uint32_t total = 0;
  while (1) {
    read = fread(buffer + total, 1, 256, file);
    if (read <= 0) {
      break;
    }
    total += read;
  }
  fclose(file);

  
  // append to buffer
  int res = SAUCE_write(buffer, total, &sauce);
  TEST_ASSERT_EQUAL(total + 129, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, total + 129, SAUCE_APPEND_PATH));
}


void should_ReplaceSAUCE_when_BufferContainsSAUCE() {
  // copy TestFile1.ans into buffer
  char buffer[1024];
  FILE* file = fopen(SAUCE_TESTFILE1_PATH, "rb");
  if (file == NULL) {
    TEST_FAIL_MESSAGE("Could not open "SAUCE_TESTFILE1_PATH);
    return;
  }

  int read;
  uint32_t total = 0;
  while (1) {
    read = fread(buffer + total, 1, 256, file);
    if (read <= 0) {
      break;
    }
    total += read;
  }
  fclose(file);

  // replace buffer's sauce
  int res = SAUCE_write(buffer, total, &sauce);
  TEST_ASSERT_EQUAL(total, res);

  TEST_ASSERT_TRUE(test_buffer_matches_expected(buffer, total, SAUCE_REPLACE_PATH));
}




// File fail cases
void should_FailToWrite_when_FileDoesNotExist() {
  int res = SAUCE_fwrite("expect/weird/File123456.txt", &sauce);
  TEST_ASSERT_EQUAL(SAUCE_EFOPEN, res);
}





int main(int argc, char** argv) {
  UNITY_BEGIN();
  
  RUN_TEST(should_WriteToFile_when_FileIsEmpty);
  RUN_TEST(should_AppendToFile_when_FileContainsContent);
  RUN_TEST(should_ReplaceSAUCE_when_FileContainsSAUCE);
  RUN_TEST(should_WriteToBuffer_when_BufferLengthIsZero);
  RUN_TEST(should_AppendToBuffer_when_BufferContainsContent);
  RUN_TEST(should_ReplaceSAUCE_when_BufferContainsSAUCE);
  RUN_TEST(should_FailToWrite_when_FileDoesNotExist);

  return UNITY_END();
}