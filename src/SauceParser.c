#include "sauce/SauceParser.h"
#include "string.h"

static char* error = NULL;



static void SAUCE_set_error(const char* message, uint32_t length) {
  if (error != NULL) {
    free(error);
  }
  error = malloc(length + 1);
  strncpy(error, message, length);
  error[length] = '\0';
}









/**
 * @brief Get an error message about the last SAUCE error that occurred.
 * 
 * @return an error message, or an empty string if no SAUCE error has occurred
 */
const char* SAUCE_get_error(void) {
  return error;
}



/**
 * @brief Fill a SAUCE struct with a default (i.e. empty) SAUCE record.
 * 
 * @param sauce SAUCE struct
 */
void SAUCE_fill_default(const SAUCE* sauce) {
  memset(sauce, 0, sizeof(SAUCE));    // zero out everything

  // set character records
  strncpy(sauce->ID, "SAUCE", 5);
  memset(sauce->Version, '0', 2);
  memset(sauce->Title, ' ', sizeof(sauce->Title));
  memset(sauce->Author, ' ', sizeof(sauce->Author));
  memset(sauce->Group, ' ', sizeof(sauce->Group));
  memset(sauce->Date, ' ', sizeof(sauce->Date));
  return;
} 





/**
 * @brief From a file, read a SAUCE record.
 * 
 * @param filepath a path to a file 
 * @param sauce a SAUCE struct that will be filled with the parsed SAUCE info
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_fread(const char* filepath, const SAUCE* sauce) {

  return 0;
}


/**
 * @brief From a given buffer, read a SAUCE record.
 * 
 * @param buffer pointer to a buffer
 * @param length the length of the buffer
 * @param sauce SAUCE struct that will be filled with the parsed SAUCE info
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_read(const char* buffer, uint32_t length, const SAUCE* sauce) {

  return 0;
}



/**
 * @brief Write a SAUCE record to a file. If the file already contains a SAUCE record, the record will be replaced.
 * 
 * @param filepath a path to a file
 * @param sauce SAUCE struct
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_fwrite(const char* filepath, const SAUCE* sauce) {

  return 0;
}


/**
 * @brief Write a SAUCE record to a buffer. If the buffer already contains a SAUCE record, the record will be replaced.
 * 
 * 
 *        To prevent a buffer overflow error, the buffer must be at least 128 bytes long (the size of a SAUCE record).
 *        If the buffer contains the EOF character, the SAUCE record will be written immediately after the EOF character. 
 *        If no EOF character exists in the buffer, the SAUCE record will be written to the beginning of the buffer. 
 * 
 * @param buffer pointer to buffer with a size of at least 128 bytes
 * @param sauce SAUCE struct
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_write(const char* buffer, const SAUCE* sauce) {

  return 0;
}










/**
 * @brief From a file, read a SAUCE CommentBlock.
 * 
 * @param filepath a path to a file 
 * @param block SAUCE_CommentBlock to be filled with the comment
 * @param lines the number of lines to read
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_fread(const char* filepath, const SAUCE_CommentBlock* block, uint8_t lines) {

  return 0;
}


/**
 * @brief From a given buffer, read a SAUCE CommentBlock.
 * 
 * @param buffer pointer to a buffer
 * @param length the length of the buffer
 * @param block SAUCE_CommentBlock to be filled with the comment
 * @param lines the number of lines to read
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_read(const char* buffer, uint32_t length, const SAUCE_CommentBlock* block, uint8_t lines) {

  return 0;
}



/**
 * @brief Add a SAUCE CommentBlock to a file. This will update the Comments field of
 *        the file's SAUCE record with the number of Comment Lines. If the file already contains
 *        a Commentblock, the block will be replaced.
 * 
 * @param filepath a path to a file
 * @param comment comment to be written
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_fwrite_open(const char* filepath, const char* comment) {

  return 0;
}


/**
 * @brief Add a SAUCE CommentBlock to a buffer. This will update the Comments field of
 *        the buffer's SAUCE record with the number of Comment Lines. If the buffer already contains
 *        a CommentBlock, the block will be replaced. 
 * 
 * 
 *        To prevent a buffer overflow error, the size of the buffer must be at least `SAUCE_COMMENT_BLOCK_SIZE(lines)` + 128 (the size of a SAUCE record).
 *        If the buffer contains the EOF character, the SAUCE CommentBlock will be written immediately after the EOF character.
 *        If no EOF character exists in the buffer, the SAUCE CommentBlock will be written immediately before the SAUCE record.
 * 
 * @param buffer pointer to buffer
 * @param comment comment to be written
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_write(const char* buffer, const char* comment) {

  return 0;
}








/**
 * @brief Remove a SAUCE record from a file, along with the SAUCE CommentBlock if it exists.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_fremove(const char* filepath) {

  return 0;
}


/**
 * @brief Remove a SAUCE record from a buffer, along with the SAUCE CommentBlock if it exists.
 * 
 * @param buffer pointer to buffer
 * @param endIndex the index immediately after the last byte of data in the buffer
 * @return the new endIndex of the buffer. On error, a negative number is returned
 */
int SAUCE_remove(const char* buffer, uint32_t endIndex) {

  return 0;
}


/**
 * @brief Remove a SAUCE CommentBlock from a file. This will update the Comments field of
 *        the file's SAUCE record to 0.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_fremove(const char* filepath) {

  return 0;
}


/**
 * @brief Remove a SAUCE CommentBlock from a buffer. This will update the Comments field of
 *        the buffer's SAUCE record to 0.
 * 
 * @param buffer pointer to buffer
 * @param endIndex the index immediately after the last byte of data in the buffer
 * @return the new endIndex of the buffer. On error, a negative number is returned
 */
int SAUCE_Comment_remove(const char* buffer, uint32_t endIndex) {

  return 0;
}








