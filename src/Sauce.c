#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "Sauce.h"

// Local constants
#define FILE_BUF_READ_SIZE      256   


// Static asserts
#define SAUCE_STATIC_ASSERT(condition, message) \
    typedef char STATIC_ASSERT_FAILED__##message[1]; typedef char STATIC_ASSERT_FAILED__##message[(condition)?1:0];

// Assert that the SAUCE struct must be exactly 128 bytes large, which can be achieved by packing the struct
SAUCE_STATIC_ASSERT(sizeof(SAUCE) == 128, sizeof_SAUCE_struct_must_be_128_bytes);



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
 * @brief Replace a record contained in buffer. The buffer must point to the beginning
 *        of the record. The comments field will not be changed.
 * 
 * @param buffer a buffer of length `SAUCE_RECORD_SIZE`
 * @param sauce the SAUCE record to write
 */
static void SAUCE_buffer_replace_record(char* buffer, const SAUCE* sauce) {
  // save Comments field
  uint8_t lines = ((SAUCE*)buffer)->Comments;

  // replace the record
  memcpy(buffer, "SAUCE", 5);
  memcpy(&buffer[5], ((uint8_t*)sauce)+5, SAUCE_RECORD_SIZE - 5);
  
  // set comments to original value
  ((SAUCE*)buffer)->Comments = lines;
}


/**
 * @brief Find a record in a file. If the last 128 bytes of the file are a record, the record and the byte immediately before the record,
 *        if there is such a byte, will be copied to the beginning of `record`. If there is no record, the last byte of the file,
 *        if the file is not empty, will be copied to the beginning of `record`. `filesize` will be set to the file's total length. 
 *        
 *        
 * 
 * @param file FILE pointer
 * @param record array of length SAUCE_RECORD_SIZE + 1
 * @param filesize size of file to be set; can be NULL; will not be set if SAUCE_EFFAIL is returned
 * @return 0 on success. If there is no record, SAUCE_ERMISS will be returned. If the file was empty,
 *         SAUCE_EEMPTY will be returned. Any other error codes that are returned indicate that the file could not be read.
 */
static int SAUCE_file_find_record(FILE* file, char* record, uint32_t* filesize) {
  rewind(file);
  
  char buffer[FILE_BUF_READ_SIZE*2];
  char* curr = &buffer[FILE_BUF_READ_SIZE]; // set curr to middle of buffer
  memset(buffer, 0, FILE_BUF_READ_SIZE*2);

  size_t read = 0;
  uint32_t total = 0;

  while(1) {
    read = fread(curr, 1, FILE_BUF_READ_SIZE, file);
    total += read;
    if (read < FILE_BUF_READ_SIZE) {
      if (total == read) {
        memcpy(buffer, curr, FILE_BUF_READ_SIZE);
      }
      if (feof(file)) {
        break;
      }
      SAUCE_set_error("Failed to read from file");
      return SAUCE_EFFAIL;
    }

    memcpy(buffer, curr, FILE_BUF_READ_SIZE);
  }

  // set file size
  if (filesize != NULL) *filesize = total;

  // check for empty file
  if (total == 0) return SAUCE_EEMPTY;

  // check for short file
  if (total < SAUCE_RECORD_SIZE) {
    record[0] = curr[total-1];
    return SAUCE_ERMISS;
  }

  // copy record into buffer
  if (total == SAUCE_RECORD_SIZE) {
    // total is exactly record size
    memcpy(record, buffer, SAUCE_RECORD_SIZE);
  } else if (total < FILE_BUF_READ_SIZE) {
    // total is less than FILE_BUF_READ_SIZE
    memcpy(record, buffer, SAUCE_RECORD_SIZE + 1);
  } else {
    // piece together record, length is FILE_BUF_READ_SIZE + read
    memcpy(record, (buffer + FILE_BUF_READ_SIZE + read - SAUCE_RECORD_SIZE - 1), SAUCE_RECORD_SIZE + 1);
  }

  // check for SAUCE id
  if (total == SAUCE_RECORD_SIZE) {
    if (memcmp(record, "SAUCE", 5) == 0) return 0;
  } else {
    if (memcmp(record+1, "SAUCE", 5) == 0) return 0;
  }

  // record does not exist
  record[0] = record[SAUCE_RECORD_SIZE];
  return SAUCE_ERMISS;
}


/**
 * @brief Find a comment in a file. If the comment is found in a file, the comment and the byte immediately 
 *        before the comment, if such a byte exists, will be copied to the beginning of `comment`.
 * 
 * @param file FILE pointer
 * @param comment comment buffer to be filled; length must be at least `SAUCE_COMMENT_BLOCK_SIZE(lines) + 1`
 * @param filesize the size/length of the file
 * @param lines the number of comment lines from the file's record
 * @return 0 on success. If the comment ID couldn't be found, then SAUCE_ECMISS will be returned.
 *         Any other returned errors indicate that the file could not be read or could not possibly contain a comment.
 */
static int SAUCE_file_find_comment(FILE* file, char* comment, uint32_t filesize, uint8_t lines) {
  rewind(file);

  // check if file is too short
  if (filesize < SAUCE_TOTAL_SIZE(lines)) {
    SAUCE_set_error("File is too short to contain a comment with %d lines", lines);
    return SAUCE_ESHORT;
  }

  // seek to byte before comment, if possible
  if (filesize > SAUCE_TOTAL_SIZE(lines) + 1) {
    if (fseek(file, filesize - SAUCE_TOTAL_SIZE(lines) - 1, SEEK_SET) < 0) {
      SAUCE_set_error("Failed to seek to byte before comment in file");
      return SAUCE_EFFAIL;
    }
  }

  // read comment and possibly the byte immediately before
  int read = fread(comment, 1, SAUCE_COMMENT_BLOCK_SIZE(lines) + 1, file);
  
  // check for comment id
  if (memcmp(((filesize == SAUCE_TOTAL_SIZE(lines)) ? comment : comment+1), "COMNT", 5) == 0) {
    return 0;
  }

  // failed to find comment id
  return SAUCE_ECMISS;
}


/**
 * @brief Append a record to a file. File will be opened in "ab" mode. An eof character will
 *        also be included.
 * 
 * @param filepath path to file
 * @param sauce the record to append
 * @return 0 on success. On error, a negative error code is returned.
 */
static int SAUCE_file_append_record(const char* filepath, const SAUCE* sauce) {
  size_t write;

  FILE* file = fopen(filepath, "ab");
  if (file == NULL) {
    SAUCE_set_error("Failed to open %s for appending", filepath);
    return SAUCE_EFOPEN;
  }

  // write an eof character
  uint8_t eofchar = SAUCE_EOF_CHAR;
  write = fwrite(&eofchar, 1, 1, file);
  if (write != 1) {
    fclose(file);
    SAUCE_set_error("Failed to append an EOF character to %s", filepath);
    return SAUCE_EFFAIL;
  }

  char record[SAUCE_RECORD_SIZE];
  memcpy(record, "SAUCE", 5);
  memcpy(record+5, &(sauce->Version), SAUCE_RECORD_SIZE - 5);
  ((SAUCE*)(record))->Comments = 0;

  // write the SAUCE record
  write = fwrite(record, 1, SAUCE_RECORD_SIZE, file);
  fclose(file);
  if (write != SAUCE_RECORD_SIZE) {
    SAUCE_set_error("Failed to append record to %s", filepath);
    return SAUCE_EFFAIL;
  }

  return 0;
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


/**
 * @brief If it is certain that an EOF character does not exist in a correct position
 *        in the buffer, then an EOF character will be inserted into the buffer.
 * 
 * @param buffer pointer to buffer containing a SAUCE record
 * @param n length of buffer
 * @return the new length of the buffer
 */
static uint32_t insert_eof_char(char* buffer, uint32_t n) {
  if (n < SAUCE_RECORD_SIZE) return n;
  if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], "SAUCE", 5) != 0) return n;


  uint8_t lines = ((SAUCE*)(&buffer[n-SAUCE_RECORD_SIZE]))->Comments;
  if (lines > 0) {
    // comment could exist, check if n is large enough to contain EOF
    if (n > SAUCE_RECORD_SIZE + SAUCE_COMMENT_BLOCK_SIZE(lines)) {
      uint32_t comment_index = n - (SAUCE_RECORD_SIZE + SAUCE_COMMENT_BLOCK_SIZE(lines));
      // check for COMNT id and if EOF character doesn't exist before it
      if (memcmp(&buffer[comment_index], "COMNT", 5) == 0 && buffer[comment_index-1] != SAUCE_EOF_CHAR) {
        // move SAUCE data forward 1 byte
        memmove(&buffer[comment_index + 1], &buffer[comment_index], SAUCE_RECORD_SIZE + SAUCE_COMMENT_BLOCK_SIZE(lines));
        buffer[comment_index] = SAUCE_EOF_CHAR; // insert EOF
        return n + 1;
      } 
    }
  } else {
    // no comment exists, look immediately before record
    if (n == SAUCE_RECORD_SIZE) {
      memmove(&buffer[1], buffer, SAUCE_RECORD_SIZE);
      buffer[0] = SAUCE_EOF_CHAR;
      return n + 1;
    }
    if (buffer[n - SAUCE_RECORD_SIZE - 1] != SAUCE_EOF_CHAR) {
      // move record forward 1 byte
      memmove(&buffer[n - SAUCE_RECORD_SIZE + 1], &buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);
      buffer[n - SAUCE_RECORD_SIZE] = SAUCE_EOF_CHAR; // insert EOF
      return n + 1;
    } 
  }

  // either an EOF already existed or it was uncertain
  return n;
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
  // null checks
  if (filepath == NULL) {
    SAUCE_set_error("Filepath was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_set_error("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  // open file for reading
  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    SAUCE_set_error("Failed to open %s", filepath);
    return SAUCE_EFOPEN;
  }

  // get the record from a file
  char record[SAUCE_RECORD_SIZE + 1];
  uint32_t filesize = 0;
  int res = SAUCE_file_find_record(file, record, &filesize);
  if (res == SAUCE_EFFAIL) {
    fclose(file);
    SAUCE_set_error("Failed to read record from %s", filepath);
    return SAUCE_EFFAIL;
  }

  // if record was not found, just append the new record
  if (res == SAUCE_ERMISS || res == SAUCE_EEMPTY) {
    fclose(file);
    return SAUCE_file_append_record(filepath, sauce);
  }

  // determine where the record starts
  uint8_t record_start = 1;
  if (res == 0 && memcmp(record, "SAUCE", 5) == 0) record_start = 0;

  // initialize operating buffer
  uint8_t lines = ((SAUCE*)(&record[record_start]))->Comments;
  uint32_t bufLen = SAUCE_TOTAL_SIZE(lines) + 1;
  char* buffer = malloc(bufLen);
  buffer[0] = SAUCE_EOF_CHAR;

  int eof_exists = 0;
  if (lines > 0) {
    // record claims there is a comment, retrieve it
    char* comment = malloc(SAUCE_COMMENT_BLOCK_SIZE(lines) + 1);
    res = SAUCE_file_find_comment(file, comment, filesize, lines);
    if (res == SAUCE_ECMISS) {
      fclose(file);
      free(buffer);
      free(comment);
      SAUCE_set_error("Record in %s indicated that %d comment lines existed, but the comment id could not be found", filepath, lines);
      return SAUCE_ECMISS;
    } else if (res < 0) {
      fclose(file);
      free(buffer);
      free(comment);
      return res;
    }

    if (comment[0] == SAUCE_EOF_CHAR) eof_exists = 1;

    // copy comment and record to buffer
    if (memcmp(comment, "COMNT", 5) == 0) {
      memcpy(buffer+1, comment, SAUCE_COMMENT_BLOCK_SIZE(lines));
    } else {
      memcpy(buffer, comment, SAUCE_COMMENT_BLOCK_SIZE(lines)+1);
    }
    memcpy(buffer+1+SAUCE_COMMENT_BLOCK_SIZE(lines), &record[record_start], SAUCE_RECORD_SIZE);
    free(comment);
  } else {
    // there is only a record, copy it to buffer
    if (record[0] == SAUCE_EOF_CHAR) eof_exists = 1;
    memcpy(buffer+1, &record[record_start], SAUCE_RECORD_SIZE);
  }

  fclose(file);


  // write the record to the buffer
  buffer[0] = SAUCE_EOF_CHAR; // first char always EOF
  SAUCE_buffer_replace_record(buffer+bufLen-SAUCE_RECORD_SIZE, sauce);

  // open file for reading/writing
  file = fopen(filepath, "rb+");
  if (file == NULL) {
    free(buffer);
    SAUCE_set_error("Failed to open %s for reading & writing", filepath);
    return SAUCE_EFOPEN;
  }

  // seek to write position
  if (eof_exists) {
    if (fseek(file, filesize - bufLen, SEEK_SET) < 0) { // seek to eof char
      fclose(file);
      free(buffer);
      SAUCE_set_error("Failed to seek to eof character in %s", filepath);
      return SAUCE_EFFAIL;
    }
  } else {
    if (fseek(file, filesize - bufLen + 1, SEEK_SET) < 0) { // seek start of comment/record
      fclose(file);
      free(buffer);
      SAUCE_set_error("Failed to seek to start of SAUCE data in %s", filepath);
      return SAUCE_EFFAIL;
    }
  }

  // write the new buffer to the file
  size_t write = fwrite(buffer, 1, bufLen, file);
  fclose(file);
  free(buffer);
  if (write != bufLen) {
    SAUCE_set_error("Failed to write to %s", filepath);
    return SAUCE_EFFAIL;
  }

  return 0;
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
int SAUCE_write(char* buffer, uint32_t n, const SAUCE* sauce) {
  // null checks
  if (buffer == NULL) {
    SAUCE_set_error("Buffer was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_set_error("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  uint32_t len = n;

  // check the size of the buffer
  if (n >= SAUCE_RECORD_SIZE) {
    // look for SAUCE id
    if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], "SAUCE", 5) == 0) {
      goto replace; // there is a record, replace it
    }
    goto append; // there is no record, append the new record
  } else {
    goto append; // buffer to short to have record, append the new record
  }


  // ===== Append a new record =====
  append:
    // add eof
    buffer[len] = SAUCE_EOF_CHAR;
    len++;

    memcpy(&buffer[len], "SAUCE", 5);
    len += 5;
    memcpy(&buffer[len], ((uint8_t*)sauce)+5, SAUCE_RECORD_SIZE - 5);
    len += (SAUCE_RECORD_SIZE - 5);

    // set the Comments field to 0
    ((SAUCE*)(&buffer[len-SAUCE_RECORD_SIZE]))->Comments = 0;

    return len;
  // ===== end of append section =====

  // ===== Replace the record =====
  replace:
    // save Comments field
    uint8_t lines = ((SAUCE*)(&buffer[len-SAUCE_RECORD_SIZE]))->Comments;

    // replace the record
    memcpy(&buffer[len - SAUCE_RECORD_SIZE], "SAUCE", 5);
    memcpy(&buffer[len - SAUCE_RECORD_SIZE + 5], ((uint8_t*)sauce)+5, SAUCE_RECORD_SIZE - 5);
    
    // set comments to original value
    ((SAUCE*)(&buffer[len-SAUCE_RECORD_SIZE]))->Comments = lines;

    // attempt to insert an EOF character
    len = insert_eof_char(buffer, len);

    return len;
  // ===== end of replace section =====
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
