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
  memset(string, ' ', 128);
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
  memcpy(expected.Group, "SomeGroup", 9);
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


SAUCE* test_get_testfile3_expected_record() {
  return test_get_testfile2_expected_record();
}





int test_file_matches_expected(const char* actual_filepath, const char* expected_filepath) {
  // open the actual file
  FILE* actual = fopen(actual_filepath, "rb");
  if (actual == NULL) {
    fprintf(stderr, "Failed to open %s", actual_filepath);
    return 0;
  }

  // open the expected file
  FILE* expected = fopen(expected_filepath, "rb");
  if (expected == NULL) {
    fprintf(stderr, "Failed to open %s", expected_filepath);
    fclose(actual);
    return 0;
  }

  // get the file sizes
  long actualSize = getFileSize(actual);
  long expectedSize = getFileSize(expected);

  // define file buffers
  char actualBuf[actualSize];
  char expectedBuf[expectedSize];

  // compare file sizes
  if (expectedSize != actualSize) {
    fprintf(stderr, "Expected file size != actual file size");
    goto failed;
  }


  // get actual file contents
  int read = fread(actualBuf, 1, actualSize, actual);
  if (read != actualSize) {
    fprintf(stderr, "Failed to read all of actual file");
    goto failed;
  }

  // get expected file contents
  read = fread(expectedBuf, 1, expectedSize, expected);
  if (read != expectedSize) {
    fprintf(stderr, "Failed to read all of expected file");
    goto failed;
  }

  // compare file contents
  if (memcmp(expectedBuf, actualBuf, expectedSize) != 0) {
    fprintf(stderr, "Failed to read all of actual file");
    goto failed;
  }

  // success, return true
  fclose(actual);
  fclose(expected);
  return 1;


  // failure case
  failed:
  fclose(actual);
  fclose(expected);
  return 0;
}


int test_buffer_matches_expected(const char* buffer, uint32_t n, const char* expected_filepath) {
  FILE* expected = fopen(expected_filepath, "rb");
  if (expected == NULL) {
    fprintf(stderr, "Failed to open %s\n", expected_filepath);
    return 0;
  }

  long expectedSize = getFileSize(expected);

  char expectedBuf[expectedSize];

  // compare file sizes
  if (expectedSize != n) {
    fprintf(stderr, "Expected file size != buffer length\n");
    goto failed;
  }

  // get file contents
  int read = fread(expectedBuf, 1, expectedSize, expected);
  if (read != expectedSize) {
    fprintf(stderr, "Failed to read all of expected file %s\n", expected_filepath);
    goto failed;
  }

  // get file contents
  if (memcmp(expectedBuf, buffer, expectedSize) != 0) {
    fprintf(stderr, "Buffer did not match %s\n", expected_filepath);
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


int copy_file(const char* src, const char* dest) {
  FILE* srcFile = fopen(src, "rb");
  if (srcFile == NULL) {
    fprintf(stderr, "Could not open %s", src);
    return -1;
  }

  FILE* destFile = fopen(dest, "wb");
  if (destFile == NULL) {
    fprintf(stderr, "Could not open %s", dest);
    fclose(srcFile);
    return -1;
  }


  // copy the file over
  char buffer[1024];
  int read = 1;
  int write = 0;
  while(read > 0) {
    read = fread(buffer, 1, 1024, srcFile);
    if (read <= 0) {
      break;
    }
    write = fwrite(buffer, 1, read, destFile);
    if (write != read) {
      fclose(srcFile);
      fclose(destFile);
      return -1;
    }
  }


  fclose(srcFile);
  fclose(destFile);
  return 0;
}



int copy_file_into_buffer(const char* srcFile, char* buffer) {
  FILE* file = fopen(srcFile, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open %s", srcFile);
    return -1;
  }

  int read;
  int total = 0;
  while (1) {
    read = fread(buffer + total, 1, 256, file);
    if (read <= 0) {
      break;
    }
    total += read;
  }
  
  fclose(file);
  return total;
}
