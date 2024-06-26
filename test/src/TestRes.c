#include "TestRes.h"


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
  return 0;
}

SAUCE_CommentBlock* test_get_testfile1_expected_comment() {
  return 0;
}