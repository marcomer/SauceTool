#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "Sauce.h"

// Static asserts
static_assert(sizeof(SAUCE) == SAUCE_RECORD_SIZE, "size of SAUCE struct should be exactly 128 bytes");


// The SAUCE error message
static char* error_msg = NULL;


/**
 * @brief Set the current error message using the `printf()` family formatting scheme.
 * 
 * @param format format string
 * @param ... optional arguments to be formatted
 */
static void SAUCE_set_error(const char* format, ...) {
  va_list ap;

  // free the error message
  if (error_msg != NULL) {
    free(error_msg);
    error_msg = NULL;
  }

  //TODO: if using Visual Studio, must be at least 2015
  //TODO: how do I require that?

  // get the length of the formatted string
  va_start(ap, format);
  int len = vsnprintf(NULL, 0, format, ap);
  va_end(ap);
  if (len < 0) {
    error_msg = malloc(128);
    snprintf(error_msg, 128, "FATAL: vsnprintf failed to get the formatted error message length");
    return;
  }

  // malloc and copy the formatted string
  error_msg = malloc(len + 1);
  va_start(ap, format);
  int res = vsnprintf(error_msg, len + 1, format, ap);
  va_end(ap);
  if (res < 0) {
    free(error_msg);
    error_msg = malloc(128);
    snprintf(error_msg, 128, "FATAL: vsnprintf failed to write the formatted error message to error_msg string");
    return;
  }
  if (len != res) {
    free(error_msg);
    error_msg = malloc(128);
    snprintf(error_msg, 128, "FATAL: vsnprintf only wrote %d characters instead of expected %d characters", res, len);
    return;
  }

  return;
}


/**
 * @brief Get the size of file. Sets file pointer to the beginning
 *        of the file when done. Files over 2GB are not supported.
 * 
 * @param file open file pointer
 * @return size of the file
 */
static uint32_t get_file_size(FILE* file) {
  if (file == NULL) return 0;

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  return size;
}






// Helper Functions

/**
 * @brief Get an error message about the last SAUCE error that occurred.
 * 
 * @return an error message, or an empty string if no SAUCE error has yet to occur
 */
const char* SAUCE_get_error(void) {
  return error_msg;
}


/**
 * @brief Fill a SAUCE struct with the default fields. ID and Version fields will be set
          to their required values. All other fields will be set to their defaults, which
          is typically 0 or spaces.
 * 
 * @param sauce a SAUCE struct to be set
 */
void SAUCE_set_default(SAUCE* sauce) {
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
uint8_t SAUCE_num_lines(const char* string) {
  int lines = 0;
  int length = strlen(string);

  if (length == 0) return 0; //handle empty string
  if (length >= UINT8_MAX * SAUCE_COMMENT_LINE_LENGTH) return UINT8_MAX; // handle largest possible string

  while (length > 0) {
    lines++;
    length -= SAUCE_COMMENT_LINE_LENGTH;
  }

  return lines;
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
int SAUCE_fread(const char* filepath, SAUCE* sauce) {
  // null checks
  if (filepath == NULL) {
    SAUCE_set_error("Filepath was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_set_error("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  // open file
  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    SAUCE_set_error("Could not open %s", filepath);
    return SAUCE_EFOPEN;
  }

  // check file size
  uint32_t fileSize;
  if ((fileSize = get_file_size(file)) < SAUCE_RECORD_SIZE) {
    fclose(file);
    SAUCE_set_error("%s has a file size of %u, which is too short to contain a record", filepath, fileSize);
    return SAUCE_ESHORT;
  }

  // seek to the last 128 bytes
  if (fseek(file, -SAUCE_RECORD_SIZE, SEEK_END) < 0) {
    fclose(file);
    SAUCE_set_error("Failed to seek to record in %s", filepath);
    return SAUCE_EFFAIL;
  }

  // read last 128 bytes into buffer and close the file
  char buffer[SAUCE_RECORD_SIZE];
  int read = fread(buffer, 1, SAUCE_RECORD_SIZE, file);
  fclose(file);
  if (read < SAUCE_RECORD_SIZE) {
    SAUCE_set_error("Failed to read 128 bytes from %s", filepath);
    return SAUCE_EFFAIL;
  }

  // find the SAUCE id
  if (memcmp(buffer, "SAUCE", 5) != 0) {
    SAUCE_set_error("Could not find SAUCE id in %s", filepath);
    return SAUCE_ERMISS;
  }

  // copy the record
  memcpy(sauce, buffer, SAUCE_RECORD_SIZE);

  return 0;
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
int SAUCE_Comment_fread(const char* filepath, SAUCE_CommentBlock* block, uint8_t nLines) {
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
int SAUCE_read(const char* buffer, uint32_t n, SAUCE* sauce) {
  // null checks
  if (buffer == NULL) {
    SAUCE_set_error("Buffer was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_set_error("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  // check if n is too short
  if (n < SAUCE_RECORD_SIZE) {
    SAUCE_set_error("The buffer length of %u is too short to contain a record", n);
    return SAUCE_ESHORT;
  }


  // find the SAUCE id
  if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], "SAUCE", 5) != 0) {
    SAUCE_set_error("Could not find the SAUCE id in the buffer");
    return SAUCE_ERMISS;
  }

  // copy the record
  memcpy(sauce, &buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);

  return 0;
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
int SAUCE_Comment_read(const char* buffer, uint32_t n, SAUCE_CommentBlock* block, uint8_t nLines) {
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
