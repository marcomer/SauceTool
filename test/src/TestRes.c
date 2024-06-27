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