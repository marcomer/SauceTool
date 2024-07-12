#include "unity.h"
#include "Sauce.h"
#include "TestRes.h"
#include <string.h>

void setUp() {}

void tearDown() {}



// SAUCE_num_lines Tests

void SAUCE_num_lines_test_should_Return0_when_StringIsEmpty() {
  int result = SAUCE_num_lines("");
  TEST_ASSERT_EQUAL_INT(0, result);
}


void SAUCE_num_lines_test_should_Return255_when_StringIsTooLarge() {
  char largeString[257 * 64];
  memset(largeString, 'a', 257 * 64);
  largeString[(257 * 64) - 1] = '\0';

  int result = SAUCE_num_lines(largeString);
  TEST_ASSERT_EQUAL_INT(255, result);
}


void SAUCE_num_lines_test_should_Return1_when_StringLenLessThanOrEqualTo64() {
  char string[65];
  memset(string, 'b', 65);
  string[63] = '\0';

  int result = SAUCE_num_lines(string);
  TEST_ASSERT_EQUAL_INT(1, result);

  memset(string, 'b', 64);
  string[64] = '\0';

  result = SAUCE_num_lines(string);
  TEST_ASSERT_EQUAL_INT(1, result);
}


void SAUCE_num_lines_test_should_Return2_when_StringLenGreaterThan64() {
  char string[128];
  memset(string, 'c', 128);
  string[65] = '\0';

  int result = SAUCE_num_lines(string);
  TEST_ASSERT_EQUAL_INT(2, result);
}


void SAUCE_num_lines_test_should_Return255_when_StringLenIsMax() {
  char string[255 * 64 + 1];
  memset(string, 'd', 255 * 64 + 1);
  string[255*64] = '\0';

  int result = SAUCE_num_lines(string);
  TEST_ASSERT_EQUAL_INT(255, result);
}


void SAUCE_num_lines_test_should_Return255_when_StringLenIsOneLessThanMax() {
  char string[255 * 64];
  memset(string, 'e', 255 * 64);
  string[(255*64)-1] = '\0';

  int result = SAUCE_num_lines(string);
  TEST_ASSERT_EQUAL_INT(255, result);
}




// SAUCE_set_default tests

void SAUCE_set_default_test_should_SetSAUCEStructToCorrectValues() {
  SAUCE sauce;
  SAUCE_set_default(&sauce);
  TEST_ASSERT_TRUE(SAUCE_equal(&sauce, &default_record));
}



// Main test fixture
int main(int argc, char** argv) {
  UNITY_BEGIN();

  // SAUCE_num_lines() tests
  RUN_TEST(SAUCE_num_lines_test_should_Return0_when_StringIsEmpty);
  RUN_TEST(SAUCE_num_lines_test_should_Return255_when_StringIsTooLarge);
  RUN_TEST(SAUCE_num_lines_test_should_Return1_when_StringLenLessThanOrEqualTo64);
  RUN_TEST(SAUCE_num_lines_test_should_Return2_when_StringLenGreaterThan64);
  RUN_TEST(SAUCE_num_lines_test_should_Return255_when_StringLenIsMax);
  RUN_TEST(SAUCE_num_lines_test_should_Return255_when_StringLenIsOneLessThanMax);

  // SAUCE_set_default() tests
  RUN_TEST(SAUCE_set_default_test_should_SetSAUCEStructToCorrectValues);

  return UNITY_END();
}