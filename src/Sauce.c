#include "Sauce.h"
#include <string.h>

// Helper Functions

/**
 * @brief Get an error message about the last SAUCE error that occurred.
 * 
 * @return an error message, or an empty string if no SAUCE error has yet to occur
 */
const char* SAUCE_get_error(void) {
  return "";
}


/**
 * @brief Fill a SAUCE struct with the default fields. ID and Version fields will be set
          to their required values. All other fields will be set to their defaults, which
          is typically 0 or spaces.
 * 
 * @param sauce a SAUCE struct to be set
 */
void SAUCE_set_default(const SAUCE* sauce) {
  memset(sauce, 0, sizeof(SAUCE)); // zero everything out

  memcpy(sauce->ID, "SAUCE", 5);
  memcpy(sauce->Version, "00", 2);

  // set the character fields to spaces
  memset(sauce->Title, ' ', sizeof(sauce->Title));
  memset(sauce->Author, ' ', sizeof(sauce->Author));
  memset(sauce->Group, ' ', sizeof(sauce->Group));
  memset(sauce->Date, ' ', sizeof(sauce->Date));
}


/**
 * @brief Determine how many comment lines a string will need in order to
 *        place it in a CommentBlock.
 * 
 * @param string a null-terminated string
 * @return how many lines the string will need
 */
int SAUCE_num_comment_lines(const char* string) {
  return 0;
}





// Read Functions

/**
 * @brief From a file, read a SAUCE record into `sauce`.
 * 
 * @param filepath a path to a file 
 * @param sauce a SAUCE struct that will be filled with the parsed SAUCE record
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_fread(const char* filepath, const SAUCE* sauce) {
  return -1;
}


/**
 * @brief From a file, read `nLines` of a SAUCE CommentBlock into `block`.
 * 
 * @param filepath a path to a file 
 * @param block a SAUCE_CommentBlock to be filled with the comment
 * @param nLines the number of lines to read
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_fread(const char* filepath, const SAUCE_CommentBlock* block, uint8_t nLines) {
  return -1;
}


/**
 * @brief From the first `n` bytes of a buffer, read a SAUCE record into `sauce`.
 * 
 * @param buffer pointer to a buffer
 * @param n the length of the buffer
 * @param sauce a SAUCE struct that will be filled with the parsed SAUCE record
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_read(const char* buffer, uint32_t n, const SAUCE* sauce) {
  return -1;
}


/**
 * @brief From the first `n` bytes of a buffer, read `nLines` of a SAUCE CommentBlock into `block`.
 * 
 * @param buffer pointer to a buffer
 * @param n the length of the buffer
 * @param block a SAUCE_CommentBlock to be filled with the comment
 * @param nLines the number of lines to read
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_read(const char* buffer, uint32_t n, const SAUCE_CommentBlock* block, uint8_t nLines) {
  return -1;
}





// Write Functions

/**
 * @brief Write a SAUCE record to a file. If the file already contains a SAUCE record, the record will be replaced.
 *        An EOF character will be added if the file previously did not contain a SAUCE record.
 * 
 * @param filepath a path to a file
 * @param sauce a SAUCE struct
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_fwrite(const char* filepath, const SAUCE* sauce) {
  return -1;
}


/**
 * @brief Write a SAUCE CommentBlock to a file, replacing a CommentBlock if one already exists.
 *        The "Comments" field of the file's SAUCE record will be updated to the new number of comment
 *        lines.
 * 
 * @param filepath a path to a file
 * @param comment a null-terminated comment string
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_fwrite(const char* filepath, const char* comment) {
  return -1;
}


/**
 * @brief Write a SAUCE record to a buffer. 
 * 
 * 
 *        If the last 128 bytes of the buffer (bytes `n-1` to `n-128`)
 *        contain a SAUCE record, the buffer's SAUCE record will be replaced. Otherwise, an EOF character and
 *        the new SAUCE record will be appended to the buffer at index `n`.
 *        Important! - To prevent a buffer overflow error when appending a new record, 
 *        the buffer's actual size must be at least n + 129 bytes (the size of a SAUCE record including an EOF character).
 * 
 * @param buffer pointer to buffer
 * @param n the length of the buffer
 * @param sauce a SAUCE struct
 * @return On success, the new length of the buffer is returned. On error, a negative error code
 *         is returned. Use `SAUCE_get_error()` to get more info on the error.
 */
int SAUCE_write(const char* buffer, uint32_t n, const SAUCE* sauce) {
  return -1;
}


/**
 * @brief Write a SAUCE CommentBlock to a buffer, replacing a CommentBlock if one already exists.
 * 
 * 
 *        If the last 128 bytes of the buffer (bytes `n-1` to `n-128`)
 *        contain a SAUCE record, the CommentBlock will be written. Otherwise, an
 *        error will be returned. The "Comments" field of the buffer's SAUCE record
 *        will be updated to the new number of comment lines.
 *        Important! - To prevent a buffer overflow error when writing a new comment, 
 *        the buffer's actual size must be at least `n` + `SAUCE_COMMENT_BLOCK_SIZE(number of comment lines)`.
 * 
 * @param buffer pointer to buffer
 * @param n the length of the buffer
 * @param comment a null-terminated comment string
 * @return On success, the new length of the buffer is returned. On error, a negative error code
 *         is returned. Use `SAUCE_get_error()` to get more info on the error.
 */
int SAUCE_Comment_write(const char* buffer, uint32_t n, const char* comment) {
  return -1;
}





// Remove Functions

/**
 * @brief Remove a SAUCE record from a file, along with the SAUCE CommentBlock if one exists.
 *        The EOF character will be removed as well.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_fremove(const char* filepath) {
  return -1;
}


/**
 * @brief Remove a SAUCE CommentBlock from a file. The "Comments" field of the file's SAUCE
 *        record will be set to 0.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_fremove(const char* filepath) {
  return -1;
}


/**
 * @brief Remove a SAUCE record from the first `n` bytes of a buffer, 
 *        along with the SAUCE CommentBlock if it exists. The EOF character will be
 *        removed as well.
 * 
 * @param buffer pointer to buffer
 * @param n the length of the buffer
 * @return On success, the new length of the buffer is returned. On error, a negative error code
 *         is returned. Use `SAUCE_get_error()` to get more info on the error.
 */
int SAUCE_remove(const char* buffer, uint32_t n) {
  return -1;
}


/**
 * @brief Remove a SAUCE CommentBlock from the first `n` bytes of a buffer.
 *        The "Comments" field of the buffer's SAUCE record will be set to 0.
 * 
 * @param buffer pointer to buffer
 * @param n the length of the buffer
 * @return On success, the new length of the buffer is returned. On error, a negative error code
 *         is returned. Use `SAUCE_get_error()` to get more info on the error.
 */
int SAUCE_Comment_remove(const char* buffer, uint32_t n) {
  return -1;
}





// Functions for performing checks

/**
 * @brief Check if a file contains valid SAUCE data. This will check the SAUCE data against
 *        the SAUCE record and CommentBlock requirements listed in the docs.
 *        TODO: add link to requirements
 * 
 * @param filepath path to a file
 * @return 1 (true) if the file contains valid SAUCE data; 0 (false) if the file does not contain valid
 *         SAUCE data. If 0 is returned, you can call `SAUCE_get_error()` to learn more about
 *         why the check failed.
 */
int SAUCE_check_file(const char* filepath) {
  return 0;
}


/**
 * @brief Check if the first `n` bytes of a buffer contain valid SAUCE data. This will check
 *        the data against the SAUCE record and CommentBlock requirements listed in the docs.
 *        TODO: add link to requirements
 * 
 * @param buffer pointer to a buffer
 * @param n the length of the buffer
 * @return 1 (true) if the buffer contains valid SAUCE data; 0 (false) if the buffer does not contain valid
 *         SAUCE data. If 0 is returned, you can call `SAUCE_get_error()` to learn more about
 *         why the check failed.
 */
int SAUCE_check_buffer(const char* buffer, uint32_t n) {
  return 0;
}


/**
 * @brief Check two SAUCE records for equality. SAUCE records are equal if
 *        each field between the SAUCE records match.
 * 
 * @param first the first SAUCE struct
 * @param second the second SAUCE struct
 * @return 1 (true) if the records are equal; 0 (false) if the records are not equal
 */
int SAUCE_equal(const SAUCE* first, const SAUCE* second) {
  if (first == second) return 1;

  if (memcmp(first->ID, second->ID, 5) != 0) return 0;
  if (memcmp(first->Version, second->Version, 2) != 0) return 0;
  if (memcmp(first->Title, second->Title, 35) != 0) return 0;
  if (memcmp(first->Author, second->Author, 20) != 0) return 0;
  if (memcmp(first->Group, second->Group, 20) != 0) return 0;
  if (memcmp(first->Date, second->Date, 8) != 0) return 0;
  
  if (strncmp(first->TInfoS, second->TInfoS, 22) != 0) return 0;

  return first->FileSize == second->FileSize &&
         first->DataType == second->DataType &&
         first->FileType == second->FileType &&
         first->TInfo1 == second->TInfo1 &&
         first->TInfo2 == second->TInfo2 &&
         first->TInfo3 == second->TInfo3 &&
         first->TInfo4 == second->TInfo4 &&
         first->Comments == second->Comments &&
         first->TFlags == second->TFlags;
}


/**
 * @brief Check two SAUCE_CommentBlocks for equality. SAUCE_CommentBlocks are equal
 *        if the content of each field match between the CommentBlocks.
 * 
 * @param first the first SAUCE struct
 * @param second the second SAUCE struct
 * @return 1 (true) if the CommentBlocks are equal; 0 (false) if the CommentBlocks are not equal
 */
int SAUCE_Comment_equal(const SAUCE_CommentBlock* first, const SAUCE_CommentBlock* second) {
  if (first == second) return 1;

  if (memcmp(first->ID, second->ID, 5) != 0) return 0;
  if (first->lines != second->lines) return 0;

  return strncmp(first->comment, second->comment, first->lines * SAUCE_COMMENT_LINE_LENGTH) == 0;
}
