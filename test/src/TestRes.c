#include "TestRes.h"
#include <string.h>


const SAUCE default_record = {
  .ID = {'S','A','U','C','E'},
  .Version = {'0','0'},
  .Title = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
  .Author = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
  .Group = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',},
  .Date = {' ',' ',' ',' ',' ',' ',' ',' ',},
  .FileSize = 0,
  .DataType = 0,
  .FileType = 0,
  .TInfo1 = 0,
  .TInfo2 = 0,
  .TInfo3 = 0,
  .TInfo4 = 0,
  .Comments = 0,
  .TFlags = 0,
  .TInfoS = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
};


// Get the file size of an open file. Will rewind the file.
long getFileSize(FILE* file) {
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);
  return size;
}




SAUCE* test_get_testfile1_expected_record() {
  static SAUCE expected;
  SAUCE_set_default(&expected);
  memcpy(expected.Title, "TestFile1", 9);
  memcpy(expected.Author, "marcomer", 8);
  memcpy(expected.Date, "20240625", 8);
  expected.FileSize = 0x18;
  expected.DataType = 1;
  expected.FileType = 1;
  expected.TInfo1 = 0x0A;
  expected.TInfo2 = 0x02;
  expected.TInfo3 = 0;
  expected.TInfo4 = 0;
  expected.Comments = 2;
  expected.TFlags = 0x02;
  memcpy(expected.TInfoS, "IBM VGA", 7);
  
  return &expected;
}


SAUCE_CommentBlock* test_get_testfile1_expected_comment() {
  static SAUCE_CommentBlock block;
  static char string[128];
  memset(string, 0, 128);
  memcpy(block.ID, "COMNT", 5);
  block.lines = 2;
  
  memcpy(string, "This is the comments field on TestFile1. This was created using the Moebius ANSI art application.", 97);
  block.comment = string;

  return &block;
}


SAUCE* test_get_testfile2_expected_record() {
  static SAUCE expected;
  SAUCE_set_default(&expected);
  memcpy(expected.Title, "TestFile2", 9);
  memcpy(expected.Author, "marcomer", 8);
  memcpy(expected.Date, "20240627", 8);
  expected.FileSize = 0;
  expected.DataType = 1;
  expected.FileType = 1;
  expected.TInfo1 = 0x01;
  expected.TInfo2 = 0x01;
  expected.TInfo3 = 0;
  expected.TInfo4 = 0;
  expected.Comments = 0;
  expected.TFlags = 0x02;
  memcpy(expected.TInfoS, "IBM VGA", 7);
  
  return &expected;
}




int test_file_matches_expected(FILE* actual, const char* expected_filepath) {
  long actualSize = getFileSize(actual);

  FILE* expected = fopen(expected_filepath, "rb");
  if (expected == NULL) {
    fprintf(stderr, "Failed to open %s", expected_filepath);
    return 0;
  }

  long expectedSize = getFileSize(expected);

  char expectedBuf[expectedSize];
  char actualBuf[actualSize];

  // compare file sizes
  if (expectedSize != actualSize) {
    fprintf(stderr, "Expected file size != actual file size");
    goto failed;
  }

  // get file contents
  int read = fread(expectedBuf, 1, expectedSize, expected);
  if (read != expectedSize) {
    fprintf(stderr, "Failed to read all of expected file");
    goto failed;
  }

  read = fread(actualBuf, 1, actualSize, actual);
  if (read != actualSize) {
    fprintf(stderr, "Failed to read all of actual file");
    goto failed;
  }

  // get file contents
  if (memcmp(expectedBuf, actualBuf, expectedSize) != 0) {
    fprintf(stderr, "Failed to read all of actual file");
    goto failed;
  }

  // success, return true
  fclose(expected);
  return 1;


  // failure case
  failed:
  fclose(expected);
  return 0;
}


int test_buffer_matches_expected(const char* buffer, uint32_t n, const char* expected_filepath) {
  FILE* expected = fopen(expected_filepath, "rb");
  if (expected == NULL) {
    fprintf(stderr, "Failed to open %s", expected_filepath);
    return 0;
  }

  long expectedSize = getFileSize(expected);

  char expectedBuf[expectedSize];

  // compare file sizes
  if (expectedSize != n) {
    fprintf(stderr, "Expected file size != actual file size");
    goto failed;
  }

  // get file contents
  int read = fread(expectedBuf, 1, expectedSize, expected);
  if (read != expectedSize) {
    fprintf(stderr, "Failed to read all of expected file");
    goto failed;
  }

  // get file contents
  if (memcmp(expectedBuf, buffer, expectedSize) != 0) {
    fprintf(stderr, "Failed to read all of actual file");
    goto failed;
  }

  // success, return true
  fclose(expected);
  return 1;


  // failure case
  failed:
  fclose(expected);
  return 0;
}