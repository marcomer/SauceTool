#include "unity.h"
#include "Sauce.h"
#include "TestRes.h"
#include <string.h>
#include <stdio.h>

// RecordReadTest, Tests all SAUCE record read functions


static SAUCE actual;

static char testfile1_buffer[1024];
static uint32_t testfile1_length;

static char testfile2_buffer[1024];
static uint32_t testfile2_length;

void setUp() {
  SAUCE_set_default(&actual);
}


void tearDown() {}



// Success cases

void should_ReadFromFile_when_FileContainsSAUCE() {
  int res = SAUCE_fread(SAUCE_TESTFILE1_PATH, &actual);
  TEST_ASSERT_EQUAL_INT(0, res);

  TEST_ASSERT_TRUE(SAUCE_equal(test_get_testfile1_expected_record(), &actual));
}


void should_ReadFromBuffer_when_BufferContainsSAUCE() {
  int res = SAUCE_read(testfile1_buffer, testfile1_length, &actual);
  TEST_ASSERT_EQUAL_INT(0, res);

  TEST_ASSERT_TRUE(SAUCE_equal(test_get_testfile1_expected_record(), &actual));
}


void should_ReadFromFile_when_FileOnlyContainsSAUCE() {
  int res = SAUCE_fread(SAUCE_TESTFILE2_PATH, &actual);
  TEST_ASSERT_EQUAL_INT(0, res);

  TEST_ASSERT_TRUE(SAUCE_equal(test_get_testfile2_expected_record(), &actual));
}


void should_ReadFromBuffer_when_BufferSizeIs128AndContainsSAUCE() {
  TEST_ASSERT_EQUAL_INT(128, testfile2_length);
  int res = SAUCE_read(testfile2_buffer, testfile2_length, &actual);
  TEST_ASSERT_EQUAL_INT(0, res);

  TEST_ASSERT_TRUE(SAUCE_equal(test_get_testfile2_expected_record(), &actual));
}



// File failure cases

void should_FailOnReadFile_when_FileDoesNotExist() {
  int res = SAUCE_fread("res/ThisFileDNE.txt", &actual);
  TEST_ASSERT_EQUAL_INT(SAUCE_EFOPEN, res);
}


void should_FailOnReadFile_when_SAUCEIsMissing() {
  int res = SAUCE_fread(SAUCE_NOSAUCE_PATH, &actual);
  TEST_ASSERT_EQUAL_INT(SAUCE_ERMISS, res);
}


void should_FailOnReadFile_when_SAUCEPointerIsNull() {
  int res = SAUCE_fread(SAUCE_TESTFILE1_PATH, NULL);
  TEST_ASSERT_EQUAL_INT(SAUCE_ENULL, res);
}


void should_FailOnReadFile_when_FileIsTooShort() {
  int res = SAUCE_fread(SAUCE_SHORTFILE_PATH, &actual);
  TEST_ASSERT_EQUAL_INT(SAUCE_ESHORT, res);
}




// Buffer failure cases

void should_FailOnReadBuffer_when_SAUCEIsMissing() {
  char buffer[256];
  memset(buffer, 0, 256);
  memcpy(buffer, "There is no SAUCE in this buffer", 30);

  int res = SAUCE_read(buffer, 256, &actual);
  TEST_ASSERT_EQUAL_INT(SAUCE_ERMISS, res);
}


void should_FailOnReadBuffer_when_BufferLengthIsTooShort() {
  int res = SAUCE_read(testfile1_buffer, 127, &actual);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);

  res = SAUCE_read(testfile1_buffer, 0, &actual);
  TEST_ASSERT_EQUAL(SAUCE_ESHORT, res);
}


void should_FailOnReadBuffer_when_BufferIsNULL() {
  int res = SAUCE_read(NULL, 128, &actual);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}


void should_FailOnReadBuffer_when_SAUCEPointerIsNULL() {
  int res = SAUCE_read(testfile1_buffer, testfile1_length, NULL);
  TEST_ASSERT_EQUAL(SAUCE_ENULL, res);
}





int main(int argc, char** argv) {
  UNITY_BEGIN();

  // Put TestFile1 into a buffer
  FILE* testfile1 = fopen(SAUCE_TESTFILE1_PATH, "rb");
  if (testfile1 == NULL) {
    fprintf(stderr, "ERROR: Failed to open %s", SAUCE_TESTFILE1_PATH);
    return -1;
  }

  int read = 0;
  testfile1_length = 0;
  while(1) {
    read = fread(testfile1_buffer+testfile1_length, 1, 128, testfile1);
    if (read == 0) break;
    testfile1_length += read;
  }
  fclose(testfile1);

  // Put TestFile2 into a buffer
  FILE* testfile2 = fopen(SAUCE_TESTFILE2_PATH, "rb");
  if (testfile2 == NULL) {
    fprintf(stderr, "ERROR: Failed to open %s", SAUCE_TESTFILE2_PATH);
    return -1;
  }

  read = 0;
  testfile2_length = 0;
  while(1) {
    read = fread(testfile2_buffer+testfile2_length, 1, 128, testfile2);
    if (read == 0) break;
    testfile2_length += read;
  }
  fclose(testfile2);
  

  // Register tests
  RUN_TEST(should_ReadFromFile_when_FileContainsSAUCE);
  RUN_TEST(should_ReadFromBuffer_when_BufferContainsSAUCE);
  RUN_TEST(should_ReadFromFile_when_FileOnlyContainsSAUCE);
  RUN_TEST(should_ReadFromBuffer_when_BufferSizeIs128AndContainsSAUCE);
  RUN_TEST(should_FailOnReadFile_when_FileDoesNotExist);
  RUN_TEST(should_FailOnReadFile_when_SAUCEIsMissing);
  RUN_TEST(should_FailOnReadFile_when_SAUCEPointerIsNull);
  RUN_TEST(should_FailOnReadFile_when_FileIsTooShort);
  RUN_TEST(should_FailOnReadBuffer_when_SAUCEIsMissing);
  RUN_TEST(should_FailOnReadBuffer_when_BufferLengthIsTooShort);
  RUN_TEST(should_FailOnReadBuffer_when_BufferIsNULL);
  RUN_TEST(should_FailOnReadBuffer_when_SAUCEPointerIsNULL);

  return UNITY_END();
}


