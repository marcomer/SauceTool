#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Sauce.h" 

// Compiler and OS defines

//TODO: consider adding MinGW?
#if defined(__GNUC__) || defined(__llvm__)
  #define USE_ATTRIBUTE
#endif 

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
  #include <unistd.h>
  #include <sys/types.h>
  #if defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112L
    #define POSIX_IS_DEFINED
    #pragma message("Compiling for POSIX system of version 200112L or higher")
  #endif
#endif

#if defined (_WIN32)
  #include <windows.h>
  #include <wchar.h>
  #define WINDOWS_IS_DEFINED
#endif


// Static asserts
#define SAUCE_STATIC_ASSERT(condition, message) \
    typedef char STATIC_ASSERT_FAILED__##message[2*!!(condition)-1]

// Assert that the SAUCE struct must be exactly 128 bytes large, which can be achieved by packing the struct
SAUCE_STATIC_ASSERT(sizeof(SAUCE) == 128, sizeof_SAUCE_struct_must_be_128_bytes);


// Local constants
#define FILE_BUF_READ_SIZE      256  

// The SAUCE error message
static char* error_msg = NULL;

// Declarations

#ifdef USE_ATTRIBUTE
__attribute__((format(printf, 1, 2)))
#endif
static int SAUCE_set_error(const char* format, ...);


// Set the current error message using the `printf()` family formatting scheme.
// Will return SAUCE_EOTHER from the calling function if SAUCE_set_error() fails.
// Note that parameters must be ordered exactly as if you were calling SAUCE_set_error().
#define SAUCE_SET_ERROR(...)     if (SAUCE_set_error(__VA_ARGS__) < 0) return SAUCE_EOTHER;


/**
 * @brief Set the current error message using the `printf()` family formatting scheme.
 * 
 * @param format format string
 * @param ... optional arguments to be formatted
 * 
 * @return 0 on success. Otherwise, a negative number is returned.
 */
static int SAUCE_set_error(const char* format, ...) {
  va_list ap;

  // free the error message
  SAUCE_clear_error();

  //TODO: if using Visual Studio, must be at least 2015
  //TODO: how do I require that?

  // get the length of the formatted string
  va_start(ap, format);
  int len = vsnprintf(NULL, 0, format, ap);
  va_end(ap);
  if (len < 0) {
    error_msg = malloc(128);
    snprintf(error_msg, 128, "SAUCE_set_error: vsnprintf failed to get the formatted error message length");
    return -1;
  }

  // malloc and copy the formatted string
  error_msg = malloc(len + 1);
  va_start(ap, format);
  int res = vsnprintf(error_msg, len + 1, format, ap);
  va_end(ap);
  if (res < 0) {
    free(error_msg);
    error_msg = malloc(128);
    snprintf(error_msg, 128, "SAUCE_set_error: vsnprintf failed to write the formatted error message to error_msg string");
    return -1;
  }
  if (len != res) {
    free(error_msg);
    error_msg = malloc(128);
    snprintf(error_msg, 128, "SAUCE_set_error: vsnprintf only wrote %d characters instead of expected %d characters", res, len);
    return -1;
  }

  return 0;
}


/**
 * @brief Clear the last error message. Will do nothing if no SAUCE error has occurred yet.
 * 
 */
void SAUCE_clear_error(void) {
  if (error_msg != NULL) {
    free(error_msg);
    error_msg = NULL;
  }
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
  memcpy(buffer, SAUCE_RECORD_ID, 5);
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
  
  char buffer[FILE_BUF_READ_SIZE*2 + 1];
  char* curr = &buffer[FILE_BUF_READ_SIZE]; // set curr to middle of buffer
  memset(buffer, 0, FILE_BUF_READ_SIZE*2 + 1);

  size_t read = 0;
  uint32_t total = 0;

  while(1) {
    read = fread(curr, 1, FILE_BUF_READ_SIZE, file);
    total += read;
    if (read < FILE_BUF_READ_SIZE) {
      if (total == read) {
        memcpy(buffer, curr, total);
        memset(curr, 0, FILE_BUF_READ_SIZE);
      }
      if (feof(file)) {
        break;
      }
      SAUCE_SET_ERROR("Failed to read from file");
      return SAUCE_EFFAIL;
    }

    memcpy(buffer, curr, FILE_BUF_READ_SIZE);
    memset(curr, 0, FILE_BUF_READ_SIZE);
  }

  // set file size
  if (filesize != NULL) *filesize = total;

  // check for empty file
  if (total == 0) return SAUCE_EEMPTY;

  // check for short file
  if (total < SAUCE_RECORD_SIZE) {
    record[0] = curr[total-1];
    return SAUCE_ESHORT;
  }

  
  uint32_t record_start = (total < FILE_BUF_READ_SIZE) ? 
                            total - SAUCE_RECORD_SIZE : 
                            (read + FILE_BUF_READ_SIZE) - SAUCE_RECORD_SIZE;

  // check for record
  if (memcmp(&buffer[record_start], SAUCE_RECORD_ID, 5) != 0) {
    record[0] = (record_start > 0) ? buffer[record_start-1] : 0;
    return SAUCE_ERMISS;
  }

  // copy record
  if (record_start > 0) {
    memcpy(record, &buffer[record_start - 1], SAUCE_RECORD_SIZE + 1);
  } else {
    memcpy(record, &buffer[record_start], SAUCE_RECORD_SIZE);
  }

  return 0;
}


/**
 * @brief Find a comment in a file. If the comment is found in a file, the comment and the byte immediately 
 *        before the comment, if such a byte exists, will be copied to the beginning of `comment`.
 * 
 * @param file FILE pointer
 * @param comment comment buffer to be filled; length must be at least `SAUCE_COMMENT_BLOCK_SIZE(lines) + 1`
 * @param filesize the size/length of the file
 * @param totalLines the total number of lines reported by the record
 * @param lines the number of comment lines to read from the file
 * @return 0 on success. If the comment ID couldn't be found, then SAUCE_ECMISS will be returned.
 *         Any other returned errors indicate that the file could not be read or could not possibly contain a comment.
 */
static int SAUCE_file_find_comment(FILE* file, char* comment, uint32_t filesize, uint8_t totalLines, uint8_t lines) {
  rewind(file);

  // check if file is too short
  if (filesize < SAUCE_TOTAL_SIZE(totalLines)) {
    SAUCE_SET_ERROR("File is too short to contain a comment with a total of %d lines", totalLines);
    return SAUCE_ESHORT;
  }

  // seek to byte before comment, if possible
  if (filesize > SAUCE_TOTAL_SIZE(totalLines) + 1) {
    if (fseek(file, filesize - SAUCE_TOTAL_SIZE(totalLines) - 1, SEEK_SET) < 0) {
      SAUCE_SET_ERROR("Failed to seek to byte before comment in file");
      return SAUCE_EFFAIL;
    }
  }

  // read comment and possibly the byte immediately before
  int read = fread(comment, 1, SAUCE_COMMENT_BLOCK_SIZE(lines) + 1, file);
  
  // check for comment id, return if found
  if (memcmp(((filesize == SAUCE_TOTAL_SIZE(lines)) ? comment : comment+1), SAUCE_COMMENT_ID, 5) == 0) {
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
    SAUCE_SET_ERROR("Failed to open %s for appending", filepath);
    return SAUCE_EFOPEN;
  }

  // write an eof character
  uint8_t eofchar = SAUCE_EOF_CHAR;
  write = fwrite(&eofchar, 1, 1, file);
  if (write != 1) {
    fclose(file);
    SAUCE_SET_ERROR("Failed to append an EOF character to %s", filepath);
    return SAUCE_EFFAIL;
  }

  char record[SAUCE_RECORD_SIZE];
  memcpy(record, SAUCE_RECORD_ID, 5);
  memcpy(record+5, &(sauce->Version), SAUCE_RECORD_SIZE - 5);
  ((SAUCE*)(record))->Comments = 0;

  // write the SAUCE record
  write = fwrite(record, 1, SAUCE_RECORD_SIZE, file);
  fclose(file);
  if (write != SAUCE_RECORD_SIZE) {
    SAUCE_SET_ERROR("Failed to append record to %s", filepath);
    return SAUCE_EFFAIL;
  }

  return 0;
}


/**
 * @brief Truncate the file by removing all SAUCE data from the end of the file.
 *        The last `totalSauceSize` bytes of the file will be removed. On success,
 *        writeRef will be set to the trucated file for writing and be positioned at end of the file.    
 * 
 * @param file FILE pointer to file to truncate; should be open for reading
 * @param filesize size of the original file
 * @param totalSauceSize size/length of the SAUCE data; this can include an EOF character
 * @param writeRef on success, will be set to the truncated file for writing and be positioned at the end of the file. 
 *                 If NULL, `writeRef` will not be set and the file will automatically be closed.
 * @return 0 on success. On error, a negative error code is returned.
 */
static int SAUCE_file_truncate(const char* filepath, uint32_t filesize, uint16_t totalSauceSize, FILE** writeRef) {
  if (filesize < totalSauceSize) {
    SAUCE_SET_ERROR("The total size of the SAUCE data cannot be greater than the filesize");
    return SAUCE_EOTHER;
  }

  if (filesize == totalSauceSize) {
    // just clear the entire file
    FILE* file = fopen(filepath, "wb");
    if (file == NULL) {
      SAUCE_SET_ERROR("Could not open %s for writing", filepath);
      return SAUCE_EFOPEN;
    }

    if (writeRef != NULL) *writeRef = file;
    else fclose(file);

    return 0;
  }

  //TODO: check for windows/posix truncate functions
  #ifdef POSIX_IS_DEFINED
    int truncateRes = truncate(filepath, filesize - totalSauceSize);
    if (truncateRes < 0) {
      SAUCE_SET_ERROR("Failed to truncate %s using POSIX truncate function", filepath);
      return SAUCE_EFFAIL;
    }

    if (writeRef != NULL) {
      FILE* file = fopen(filepath, "ab");
      if (file == NULL) {
        SAUCE_SET_ERROR("Failed to open %s for appending", filepath);
        return SAUCE_EFOPEN;
      }
      *writeRef = file;
    }
    return 0;
  #endif // end POSIX_IS_DEFINED
  #ifdef WINDOWS_IS_DEFINED
    // convert filepath to wide char string
    size_t filepathStrLen = strlen(filepath);
    wchar_t* wideString = malloc((filepathStrLen + 1) * sizeof(wchar_t));
    wideString[filepathStrLen] = L'\0';
    if (mbstowcs(wideString, filepath, filepathStrLen) != filepathStrLen) {
      free(wideString);
      SAUCE_SET_ERROR("Failed to create wide character string representation of %s filepath", filepath);
      return SAUCE_EOTHER;
    }

    // open file
    HANDLE fh;
    fh = CreateFileW(wideString, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    free(wideString);
    if (fh == INVALID_HANDLE_VALUE) {
      SAUCE_SET_ERROR("Failed to open %s using Windows CreateFileW()", filepath);
      return SAUCE_EFOPEN;
    }

    // go to beginning of SAUCE data
    LARGE_INTEGER li;
    li.QuadPart = filesize - totalSauceSize;
    if (SetFilePointerEx(fh, li, NULL, FILE_BEGIN) == 0) {
      CloseHandle(fh);
      SAUCE_SET_ERROR("SetFilePointerEx() failed to seek to beginning of SAUCE data in %s", filepath);
      return SAUCE_EFFAIL;
    }

    // truncate by setting end of file
    BOOL endOfFileRes = SetEndOfFile(fh);
    CloseHandle(fh);
    if (endOfFileRes == 0) {
      SAUCE_SET_ERROR("Failed to truncate %s using Windows SetEndOfFile()", filepath);
      return SAUCE_EFFAIL;
    }

    // set writeRef if needed
    if (writeRef != NULL) {
      FILE* file = fopen(filepath, "ab");
      if (file == NULL) {
        SAUCE_SET_ERROR("Failed to open %s for appending", filepath);
        return SAUCE_EFOPEN;
      }
      *writeRef = file;
    }
    return 0;
  #endif // end if WINDOWS_IS_DEFINED


  // open file and temp file
  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    SAUCE_SET_ERROR("Could not open %s for reading", filepath);
    return SAUCE_EFOPEN;
  }

  FILE* tempFile = tmpfile();
  if (tempFile == NULL) {
    fclose(file);
    SAUCE_SET_ERROR("Failed to open a temporary file");
    return SAUCE_EFOPEN;
  }

  // copy file into tempFile
  char buffer[FILE_BUF_READ_SIZE];
  size_t read, write, readSize;
  uint32_t total = 0;
  
  readSize = FILE_BUF_READ_SIZE;
  while (1) {
    if (filesize - totalSauceSize - total < FILE_BUF_READ_SIZE) readSize = filesize - totalSauceSize - total;
    read = fread(buffer, 1, readSize, file);
    total += read;
    if (read == 0) {
      if (feof(file)) break;
      fclose(file);
      fclose(tempFile);
      SAUCE_SET_ERROR("Failed to read from %s", filepath);
      return SAUCE_EFFAIL;
    }

    // write to tempFile
    write = fwrite(buffer, 1, read, tempFile);
    if (write != read) {
      fclose(file);
      fclose(tempFile);
      SAUCE_SET_ERROR("Failed to write to tempfile");
      return SAUCE_EFFAIL;
    }

    if (total == filesize - totalSauceSize) {
      break;
    }
  }

  // prepare for copying temp to file
  rewind(tempFile);
  file = freopen(filepath, "wb", file);
  if (file == NULL) {
    fclose(tempFile);
    SAUCE_SET_ERROR("Failed to reopen %s for writing", filepath);
    return SAUCE_EFOPEN;
  }

  // copy entire tempFile to file
  while(1) {
    read = fread(buffer, 1, FILE_BUF_READ_SIZE, tempFile);
    if (read == 0) {
      if (feof(tempFile)) break;
      fclose(file);
      fclose(tempFile);
      SAUCE_SET_ERROR("Failed to read from tempfile");
      return SAUCE_EFFAIL;
    }

    write = fwrite(buffer, 1, read, file);
    if (write != read) {
      fclose(file);
      fclose(tempFile);
      SAUCE_SET_ERROR("Failed to write to %s", filepath);
      return SAUCE_EFFAIL;
    } 
  }

  fclose(tempFile);
  if (writeRef != NULL) *writeRef = file;
  else fclose(file);
  return 0;
}





/**
 * @brief If it is certain that an EOF character does not exist in a correct position
 *        in the buffer, then an EOF character will be inserted into the buffer.
 * 
 * @param buffer pointer to buffer containing a SAUCE record
 * @param n length of buffer
 * @param lines the number of comment lines
 * @return the new length of the buffer
 */
static uint32_t insert_eof_char(char* buffer, uint32_t n, uint8_t lines) {
  if (n < SAUCE_RECORD_SIZE) return n;
  if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_ID, 5) != 0) return n;

  if (lines > 0) {
    // comment could exist, check if n is large enough to contain EOF
    if (n > SAUCE_TOTAL_SIZE(lines)) {
      uint32_t comment_index = n - SAUCE_TOTAL_SIZE(lines);
      // check for COMNT id and if EOF character doesn't exist before it
      if (memcmp(&buffer[comment_index], SAUCE_COMMENT_ID, 5) == 0 && buffer[comment_index-1] != SAUCE_EOF_CHAR) {
        // move SAUCE data forward 1 byte
        memmove(&buffer[comment_index + 1], &buffer[comment_index], SAUCE_TOTAL_SIZE(lines));
        buffer[comment_index] = SAUCE_EOF_CHAR; // insert EOF
        return n + 1;
      } 
    }
  } else {
    // no comment exists, look immediately before record
    if (n == SAUCE_RECORD_SIZE) {
      // there is only a record in the file, insert eof at beginning of buffer
      memmove(&buffer[1], buffer, SAUCE_RECORD_SIZE);
      buffer[0] = SAUCE_EOF_CHAR;
      return n + 1;
    }
    if (buffer[n - SAUCE_RECORD_SIZE - 1] != SAUCE_EOF_CHAR) {
      // there is no eof before record, insert an eof before record
      memmove(&buffer[n - SAUCE_RECORD_SIZE + 1], &buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);
      buffer[n - SAUCE_RECORD_SIZE] = SAUCE_EOF_CHAR; // insert EOF
      return n + 1;
    } 
  }

  // either an EOF already existed or it was uncertain
  return n;
}




typedef struct SAUCEInfo {
  int record_exists;      // boolean; true if the record exists, false if otherwise
  int comment_exists;     // boolean; true if the comment exists, false if otherwise
  int eof_exists;         // boolean; true if the eof char exists, false if otherwise. Will be immediately before comment/record.
  uint8_t lines;          // The number of comment lines reported in the record. Note that a positive `lines` and false `comment_exists` signals an invalid comment.
  uint32_t start;         // The starting index/position of the SAUCE data; if an eof exists, it will be immediately before this index
  uint32_t sauce_length;  // The length of the found SAUCE data. This is also the length of the `dataBuffer`.
} SAUCEInfo;

/**
 * @brief Get info about SAUCE data in a file and optionally retrieve all available SAUCE data. 
 *        See SAUCEInfo struct for what info is collected. `info` will always be set appropriately, 
 *        no matter the return condition.
 * 
 *        If `dataBuffer` is not NULL, then `dataBuffer` will be malloced
 *        and contain a copy of any found SAUCE data. Length of `dataBuffer` will be
 *        `info->sauce_length`. The `dataBuffer` will not contain the eof character,
 *        if the eof exists. `dataBuffer` will NOT be allocated if no SAUCE record was
 *        found. As long a SAUCE record is found, the `dataBuffer` will be allocated.
 * 
 *        Some info will be irrelevant if certain conditions are not met.
 *        For example, if no record exists, all other SAUCEInfo fields will be irrelevant.
 * 
 * @param filepath path to file
 * @param info SAUCEInfo struct which will be filled with info on the SAUCE data
 * @param filesizePtr will be set to the size of the file. Can be NULL.
 * @param dataBuffer buffer that will be malloced and contain the SAUCE data; if NULL, nothing will be allocated
 * @return 0 on success. A success is when a record is found or a record is found along with an optional valid comment. 
 *         On error, a negative error code is returned.
 */
static int SAUCE_file_get_info(const char* filepath, SAUCEInfo* info, uint32_t* filesizePtr, char** dataBuffer) {
  if (info == NULL) {
    SAUCE_SET_ERROR("SAUCEInfo struct was NULL");
    return SAUCE_ENULL;
  }
  memset(info, 0, sizeof(SAUCEInfo));

  if (filepath == NULL) {
    SAUCE_SET_ERROR("Filepath was NULL");
    return SAUCE_ENULL;
  }

  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    SAUCE_SET_ERROR("Failed to open %s for reading", filepath);
    return SAUCE_EFOPEN;
  }

  char record[SAUCE_RECORD_SIZE + 1];
  uint32_t filesize = 0;
  int res = SAUCE_file_find_record(file, record, &filesize);
  if (filesizePtr != NULL) *filesizePtr = filesize;
  if (res < 0) {
    fclose(file);
    switch (res) {
      case SAUCE_ERMISS:
        SAUCE_SET_ERROR("%s does not contain a record", filepath);
        break;
      case SAUCE_EEMPTY:
        SAUCE_SET_ERROR("%s is an empty file and cannot contain a record", filepath);
        break;
      case SAUCE_ESHORT:
        SAUCE_SET_ERROR("%s is too short to contain a record", filepath);
        break;
      default:
        break;
    }
    info->record_exists = 0;
    return res;
  }

  // found record, fill appropriate info
  info->record_exists = 1;
  info->start = filesize - SAUCE_RECORD_SIZE;
  info->sauce_length = SAUCE_RECORD_SIZE;
  info->eof_exists = 0;

  // get start of record and previous character
  uint8_t record_start = 1;
  if (memcmp(record, SAUCE_RECORD_ID, 5) == 0) record_start = 0;
  if (record_start == 1 && record[0] == SAUCE_EOF_CHAR) info->eof_exists = 1;
  info->lines = ((SAUCE*)(&record[record_start]))->Comments;

  // look for comment
  res = 0; // SAUCE_file_find_comment may return error. res will keep track of it.
  char* commentBuffer = NULL;
  if (info->lines == 0) {
    info->comment_exists = 0;
  } else {
    info->comment_exists = 1;
    info->eof_exists = 0;
    uint8_t linesToRead = (dataBuffer == NULL) ? 1 : info->lines;
    commentBuffer = malloc(SAUCE_COMMENT_BLOCK_SIZE(info->lines) + 1);
    res = SAUCE_file_find_comment(file, commentBuffer, filesize, info->lines, info->lines);
    if (res < 0) {
      info->comment_exists = 0;
      if (res == SAUCE_ECMISS) {
        SAUCE_set_error("Record in %s claims that %d comment lines can be read, but the comment could not be found", filepath, info->lines);
      }
    } else {
      if (commentBuffer[0] == SAUCE_EOF_CHAR) info->eof_exists = 1;
      info->start = filesize - SAUCE_TOTAL_SIZE(info->lines);
      info->sauce_length = SAUCE_TOTAL_SIZE(info->lines);
    }
  }

  fclose(file);

  if (dataBuffer == NULL) {
    // nothing else to do, data does not need to copied
    if (commentBuffer != NULL) free(commentBuffer);
    return res;
  }

  // write to the dataBuffer
  if (info->comment_exists) {
    *dataBuffer = malloc(SAUCE_TOTAL_SIZE(info->lines));
    if (memcmp(commentBuffer, SAUCE_COMMENT_ID, 5) == 0) {
      memcpy(*dataBuffer, commentBuffer, SAUCE_COMMENT_BLOCK_SIZE(info->lines));
    } else {
      memcpy(*dataBuffer, commentBuffer+1, SAUCE_COMMENT_BLOCK_SIZE(info->lines));
    }
    memcpy(&((*dataBuffer)[SAUCE_COMMENT_BLOCK_SIZE(info->lines)]), &record[record_start], SAUCE_RECORD_SIZE);
  }
  else {
    *dataBuffer = malloc(SAUCE_RECORD_SIZE);
    memcpy(*dataBuffer, &record[record_start], SAUCE_RECORD_SIZE);
  }

  if (commentBuffer != NULL) free(commentBuffer);
  return res;
}


/**
 * @brief Get info about SAUCE data in a buffer. See SAUCEInfo struct for what info is
 *        collected. `info` will always be set appropriately, no matter the return condition.
 * 
 *        Some info will be irrelevant if certain conditions are not met.
 *        For example, if no record exists, all other SAUCEInfo fields will be irrelevant.
 * 
 * @param buffer a buffer array
 * @param n the length of the buffer
 * @param info SAUCEInfo struct which will be filled with info on the SAUCE data
 * @return 0 on success. A success is when a record is found or a record is found along with an optional valid comment. 
 *         On error, a negative error code is returned.
 */
static int SAUCE_buffer_get_info(const char* buffer, uint32_t n, SAUCEInfo* info) {
  if (info == NULL) {
    SAUCE_SET_ERROR("SAUCEInfo struct was NULL");
    return SAUCE_ENULL;
  }
  memset(info, 0, sizeof(SAUCEInfo));
  
  if (buffer == NULL) {
    SAUCE_SET_ERROR("Buffer was NULL");
    return SAUCE_ENULL;
  }
  if (n == 0) {
    SAUCE_SET_ERROR("Buffer's length is zero and cannot contain a record");
    return SAUCE_EEMPTY;
  }
  if (n < SAUCE_RECORD_SIZE) {
    SAUCE_SET_ERROR("Buffer's length is too short to contain a record");
    return SAUCE_ESHORT;
  }

  // look for record
  if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_ID, 5) != 0) {
    return SAUCE_ERMISS;
  }
  info->record_exists = 1;
  info->start = n - SAUCE_RECORD_SIZE;
  info->sauce_length = SAUCE_RECORD_SIZE;

  if (n > SAUCE_RECORD_SIZE && buffer[info->start - 1] == SAUCE_EOF_CHAR) info->eof_exists = 1;
  info->lines = ((SAUCE*)(&buffer[info->start]))->Comments;

  // look for comment
  if (info->lines == 0) {
    info->comment_exists = 0;
  } else {
    info->comment_exists = 1;
    info->eof_exists = 0;
    uint32_t sauceSize = SAUCE_TOTAL_SIZE(info->lines);
    if (n < sauceSize) {
      SAUCE_SET_ERROR("Buffer is too short to contain a comment with %d lines", info->lines);
      return SAUCE_ESHORT;
    }
    if (memcmp(&buffer[n - sauceSize], SAUCE_COMMENT_ID, 5) != 0) {
      info->comment_exists = 0;
      SAUCE_SET_ERROR("Record in buffer claims that %d comment lines can be read, but the comment could not be found", info->lines);
      return SAUCE_ECMISS;
    }

    // comment found
    info->start = n - sauceSize;
    info->sauce_length = sauceSize;
    if (n > sauceSize && buffer[info->start - 1] == SAUCE_EOF_CHAR) info->eof_exists = 1;
  }

  return 0;
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

  memcpy(sauce->ID, SAUCE_RECORD_ID, 5);
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
    SAUCE_SET_ERROR("Filepath was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_SET_ERROR("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  // open file
  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    SAUCE_SET_ERROR("Could not open %s", filepath);
    return SAUCE_EFOPEN;
  }

  // get record
  char record[SAUCE_RECORD_SIZE + 1];
  uint32_t filesize = 0;
  int res = SAUCE_file_find_record(file, record, &filesize);
  fclose(file);
  if (res == SAUCE_EEMPTY) {
    SAUCE_SET_ERROR("%s is empty and cannot contain a record", filepath);
    return SAUCE_EEMPTY;
  } else if (filesize < SAUCE_RECORD_SIZE) {
    SAUCE_SET_ERROR("%s is too short to contain a record", filepath);
    return SAUCE_ESHORT;
  } else if (res == SAUCE_ERMISS) {
    SAUCE_SET_ERROR("%s does not contain a record", filepath);
    return SAUCE_ERMISS;
  } else if (res < 0) {
    return res;
  }

  // record was found, copy it into sauce
  if (memcmp(record, SAUCE_RECORD_ID, 5) == 0) {
    memcpy(sauce, record, SAUCE_RECORD_SIZE);
  } else {
    memcpy(sauce, record+1, SAUCE_RECORD_SIZE);
  }

  return 0;
}


/**
 * @brief From a file, read at most `nLines` of a SAUCE CommentBlock into `comment`.
 *        A null character will be appended onto `comment` as well.
 * 
 * 
 *        If the file does not contain a comment or the actual number of lines is less
 *        than `nLines`, then expect 0 lines or all lines to be read, respectively.
 * 
 * @param filepath a path to a file 
 * @param comment a buffer of at least size `SAUCE_COMMENT_STRING_LENGTH(nLines) + 1` that will contain the comment
 * @param nLines the number of lines to read
 * @return On success, the number of lines read. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_fread(const char* filepath, char* comment, uint8_t nLines) {
  if (filepath == NULL) {
    SAUCE_SET_ERROR("Filepath was NULL");
    return SAUCE_ENULL;
  }
  if (comment == NULL) {
    SAUCE_SET_ERROR("Comment string argument was NULL");
    return SAUCE_ENULL;
  }

  if (nLines == 0) return 0;

  SAUCEInfo info;
  char* buffer = NULL;
  int res = SAUCE_file_get_info(filepath, &info, NULL, &buffer);
  if (res < 0) {
    if (buffer != NULL) free(buffer);
    return res;
  }
  if (!info.comment_exists) {
    if (buffer != NULL) free(buffer);
    comment[0] = 0;
    return 0;
  }

  nLines = (nLines > info.lines) ? info.lines : nLines;

  // copy comment to comment string
  memcpy(comment, buffer+5, SAUCE_COMMENT_STRING_LENGTH(nLines));
  comment[SAUCE_COMMENT_STRING_LENGTH(nLines)] = 0;
  if (buffer != NULL) free(buffer);

  return nLines;
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
    SAUCE_SET_ERROR("Buffer was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_SET_ERROR("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  if (n == 0) {
    SAUCE_SET_ERROR("The buffer has a length of 0 and cannot contain a record");
    return SAUCE_EEMPTY;
  }

  if (n < SAUCE_RECORD_SIZE) {
    SAUCE_SET_ERROR("The buffer length of %u is too short to contain a record", n);
    return SAUCE_ESHORT;
  }


  // find the SAUCE id
  if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_ID, 5) != 0) {
    SAUCE_SET_ERROR("Could not find the SAUCE id in the buffer");
    return SAUCE_ERMISS;
  }

  // copy the record
  memcpy(sauce, &buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);

  return 0;
}


/**
 * @brief From the first `n` bytes of a buffer, read at most `nLines` of a SAUCE CommentBlock into `comment`.
 *        A null character will be appended onto `comment` as well.
 * 
 * 
 *        If the buffer does not contain a comment or the actual number of lines is less than nLines, 
 *        then expect 0 lines or all lines to be read, respectively.
 * 
 * @param buffer pointer to a buffer
 * @param n the length of the buffer
 * @param comment a buffer of at least size `SAUCE_COMMENT_STRING_LENGTH(nLines) + 1` that will contain the comment
 * @param nLines the number of lines to read
 * @return On success, the number of lines read. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_read(const char* buffer, uint32_t n, char* comment, uint8_t nLines) {
  if (buffer == NULL) {
    SAUCE_SET_ERROR("Buffer was null");
    return SAUCE_ENULL;
  }
  if (comment == NULL) {
    SAUCE_SET_ERROR("Comment string argment was null");
    return SAUCE_ENULL;
  }
  if (n == 0) {
    SAUCE_SET_ERROR("Buffer's length is 0, so nothing can be read");
    return SAUCE_EEMPTY;
  }
  if (n < SAUCE_RECORD_SIZE) {
    SAUCE_SET_ERROR("Buffer's length is too short to contain a record");
    return SAUCE_ESHORT;
  }

  // find the SAUCE id
  if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_ID, 5) != 0) {
    SAUCE_SET_ERROR("Could not find the SAUCE id in the buffer");
    return SAUCE_ERMISS;
  }

  // get total lines from record
  uint8_t totalLines = ((SAUCE*)(&buffer[n - SAUCE_RECORD_SIZE]))->Comments;
  if (totalLines > 0 && n < SAUCE_TOTAL_SIZE(totalLines)) {
    SAUCE_SET_ERROR("Record claims that buffer contains %u comment lines, but the buffer is too short to contain that many lines", totalLines);
    return SAUCE_ESHORT;
  }

  // determine num of lines to read
  nLines = (totalLines > nLines) ? nLines : totalLines;

  // check if no lines need to or can be read
  if (nLines == 0) {
    comment[0] = 0; // add null character
    return 0;
  }

  // check for COMNT id
  if (memcmp(&buffer[n-SAUCE_TOTAL_SIZE(totalLines)], SAUCE_COMMENT_ID, 5) != 0) {
    SAUCE_SET_ERROR("Record in buffer indicated that %u total comment lines could be read, but the comment id could not be found", totalLines);
    return SAUCE_ECMISS;
  }

  // retrieve the comment
  memcpy(comment, &buffer[n-SAUCE_TOTAL_SIZE(totalLines)+5], SAUCE_COMMENT_STRING_LENGTH(nLines));
  comment[SAUCE_COMMENT_STRING_LENGTH(nLines)] = 0;
  return nLines;
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
    SAUCE_SET_ERROR("Filepath was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_SET_ERROR("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  SAUCEInfo info;
  uint32_t filesize = 0;
  char* buffer = NULL; // Note: if allocated, will be freed by free(writeBuffer) call
  int res = SAUCE_file_get_info(filepath, &info, &filesize, &buffer);
  if (res < 0 && info.record_exists) return res;

  uint32_t bufLen = SAUCE_TOTAL_SIZE(info.lines);

  char* writeBuffer;
  if (info.record_exists) {
    // prepare to replace record
    writeBuffer = buffer;
    memcpy(writeBuffer + (bufLen - SAUCE_RECORD_SIZE + 5), &(sauce->Version), SAUCE_RECORD_SIZE-5);
    ((SAUCE*)(&writeBuffer[bufLen - SAUCE_RECORD_SIZE]))->Comments = info.lines;
  } else {
    // prepare to append record
    if (buffer != NULL) free(buffer);
    writeBuffer = malloc(SAUCE_RECORD_SIZE);
    bufLen = SAUCE_RECORD_SIZE;
    memcpy(writeBuffer, SAUCE_RECORD_ID, 5);
    memcpy(writeBuffer+5, &(sauce->Version), SAUCE_RECORD_SIZE-5);
    ((SAUCE*)writeBuffer)->Comments = 0;
  }

  // prepare file for writing
  FILE* file;
  if (info.record_exists) {
    // will need to replace record
    file = fopen(filepath, "rb+");
    if (file == NULL) {
      free(writeBuffer);
      SAUCE_SET_ERROR("Failed to open %s for reading & writing", filepath);
      return SAUCE_EFOPEN;
    }
    if (fseek(file, info.start, SEEK_SET) < 0) { // seek to beginning of SAUCE data
      fclose(file);
      free(writeBuffer);
      SAUCE_SET_ERROR("Failed to seek to eof character in %s", filepath);
      return SAUCE_EFFAIL;
    }
  } else {
    // will need to append record
    file = fopen(filepath, "ab");
    if (file == NULL) {
      free(writeBuffer);
      SAUCE_SET_ERROR("Failed to open %s for appending", filepath);
      return SAUCE_EFOPEN;
    }
  }

  // write eof if needed
  size_t write;
  if (!info.eof_exists) {
    char eof_char = SAUCE_EOF_CHAR;
    write = fwrite(&eof_char, 1, 1, file);
    if (write != 1) {
      fclose(file);
      free(writeBuffer);
      SAUCE_SET_ERROR("Failed to write eof character to %s", filepath);
      return SAUCE_EFFAIL;
    }
  }

  // write the new buffer to the file
  write = fwrite(writeBuffer, 1, bufLen, file);
  fclose(file);
  free(writeBuffer);
  if (write != bufLen) {
    SAUCE_SET_ERROR("Failed to write SAUCE data to %s", filepath);
    return SAUCE_EFFAIL;
  }

  return 0;
}


/**
 * @brief Write a SAUCE CommentBlock to a file, replacing a CommentBlock if one already exists.
 *        The "Comments" field of the file's SAUCE record will be updated to `lines`.
 *        
 * 
 * @param filepath a path to a file
 * @param comment a comment buffer that is as least `SAUCE_COMMENT_STRING_LENGTH(lines)` bytes long
 * @param lines the number of lines to write
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_fwrite(const char* filepath, const char* comment, uint8_t lines) {
  if (filepath == NULL) {
    SAUCE_SET_ERROR("Filepath was NULL");
    return SAUCE_ENULL;
  }
  if (comment == NULL) {
    SAUCE_SET_ERROR("Comment string argument was NULL");
    return SAUCE_ENULL;
  }

  if (lines == 0) return 0;

  SAUCEInfo info;
  uint32_t filesize = 0;
  char* buffer = NULL;
  int res = SAUCE_file_get_info(filepath, &info, &filesize, &buffer);
  if (res < 0 && !info.record_exists) return res; // we can continue as long as the record exists

  // copy record
  char record[SAUCE_RECORD_SIZE];
  memcpy(record, &buffer[info.sauce_length - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);

  // construct new SAUCE data
  uint32_t bufLen = SAUCE_TOTAL_SIZE(lines);
  free(buffer);
  buffer = malloc(bufLen);
  memcpy(buffer, SAUCE_COMMENT_ID, 5);
  memcpy(buffer+5, comment, SAUCE_COMMENT_STRING_LENGTH(lines));
  memcpy(buffer+SAUCE_COMMENT_BLOCK_SIZE(lines), record, SAUCE_RECORD_SIZE);
  ((SAUCE*)(&buffer[SAUCE_COMMENT_BLOCK_SIZE(lines)]))->Comments = lines;

  // prep file for writing
  FILE* file;
  if (info.comment_exists && info.lines > lines) {
    // file will be shorter, truncate it
    res = SAUCE_file_truncate(filepath, filesize, info.sauce_length, &file);
    if (res < 0) {
      free(buffer);
      return res;
    }
  } else {
    file = fopen(filepath, "rb+");
    if (file == NULL) {
      free(buffer);
      SAUCE_SET_ERROR("Failed to open %s for reading and writing", filepath);
      return SAUCE_EFOPEN;
    }
    if (fseek(file, filesize - info.sauce_length, SEEK_SET) < 0) {
      fclose(file);
      free(buffer);
      SAUCE_SET_ERROR("Failed to seek to beginning of original SAUCE data in %s", filepath);
      return SAUCE_EFFAIL;
    }
  }

  // write an eof character if needed
  size_t write;
  if (!info.eof_exists) {
    char eof_char = SAUCE_EOF_CHAR;
    write = fwrite(&eof_char, 1, 1, file);
    if (write != 1) {
      fclose(file);
      free(buffer);
      SAUCE_SET_ERROR("Failed to write eof character to %s", filepath);
      return SAUCE_EFFAIL;
    }
  }

  // write buffer to file
  res = fwrite(buffer, 1, bufLen, file);
  fclose(file);
  free(buffer);
  if (res != bufLen) {
    SAUCE_SET_ERROR("Failed to write new comment and record to %s", filepath);
    return SAUCE_EFFAIL;
  }

  return 0;
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
    SAUCE_SET_ERROR("Buffer was NULL");
    return SAUCE_ENULL;
  }
  if (sauce == NULL) {
    SAUCE_SET_ERROR("SAUCE struct was NULL");
    return SAUCE_ENULL;
  }

  uint32_t len = n;

  // check the size of the buffer
  if (n >= SAUCE_RECORD_SIZE) {
    // look for SAUCE id
    if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_ID, 5) == 0) {
      goto replace; // there is a record, replace it
    }
    goto append; // there is no record, append the new record
  } else {
    goto append; // buffer to short to have record, append the new record
  }


  // ===== Append a new record =====
  append: {
    // add eof
    buffer[len] = SAUCE_EOF_CHAR;
    len++;

    memcpy(&buffer[len], SAUCE_RECORD_ID, 5);
    len += 5;
    memcpy(&buffer[len], ((uint8_t*)sauce)+5, SAUCE_RECORD_SIZE - 5);
    len += (SAUCE_RECORD_SIZE - 5);

    // set the Comments field to 0
    ((SAUCE*)(&buffer[len-SAUCE_RECORD_SIZE]))->Comments = 0;

    return len;
  }
  // ===== end of append section =====

  // ===== Replace the record =====
  replace: {
    // save Comments field
    uint8_t lines = ((SAUCE*)(&buffer[len-SAUCE_RECORD_SIZE]))->Comments;

    // replace the record
    memcpy(&buffer[len - SAUCE_RECORD_SIZE], SAUCE_RECORD_ID, 5);
    memcpy(&buffer[len - SAUCE_RECORD_SIZE + 5], ((uint8_t*)sauce)+5, SAUCE_RECORD_SIZE - 5);
    
    // set comments to original value
    ((SAUCE*)(&buffer[len-SAUCE_RECORD_SIZE]))->Comments = lines;

    // attempt to insert an EOF character
    len = insert_eof_char(buffer, len, lines);

    return len;
  }
  // ===== end of replace section =====
}


/**
 * @brief Write a SAUCE CommentBlock to a buffer, replacing a CommentBlock if one already exists.
 *        The "Comments" field of the buffer's SAUCE record will be updated to `lines`.
 * 
 * 
 *        If the last 128 bytes of the buffer (bytes `n-1` to `n-128`)
 *        contain a SAUCE record, the CommentBlock will be written. Otherwise, an
 *        error will be returned. 
 *        Important! - To prevent a buffer overflow error when writing a new comment, 
 *        the buffer's actual size must be at least `n` + `SAUCE_COMMENT_BLOCK_SIZE(number of comment lines)`.
 * 
 * @param buffer pointer to buffer
 * @param n the length of the buffer
 * @param comment a comment buffer that is as least `SAUCE_COMMENT_STRING_LENGTH(lines)` bytes long
 * @param lines the number of lines to write
 * @return On success, the new length of the buffer is returned. On error, a negative error code
 *         is returned. Use `SAUCE_get_error()` to get more info on the error.
 */
int SAUCE_Comment_write(char* buffer, uint32_t n, const char* comment, uint8_t lines) {
  if (buffer == NULL) {
    SAUCE_SET_ERROR("Buffer was NULL");
    return SAUCE_ENULL;
  }
  if (comment == NULL) {
    SAUCE_SET_ERROR("Comment string argument was NULL");
    return SAUCE_ENULL;
  }

  if (n == 0) {
    SAUCE_SET_ERROR("Buffer's length is zero and cannot contain a record");
    return SAUCE_EEMPTY;
  }
  if (n < SAUCE_RECORD_SIZE) {
    SAUCE_SET_ERROR("Buffer's length is too short to contain a record");
    return SAUCE_ESHORT;
  }

  // find record
  if (memcmp(&buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_ID, 5) != 0) {
    SAUCE_SET_ERROR("Buffer does not contain a record");
    return SAUCE_ERMISS;
  }

  // return immediately if lines is 0
  if (lines == 0) return n;

  // check if there is already an EOF character before record
  int eof_exists = 0;
  if (n > SAUCE_RECORD_SIZE && buffer[n - SAUCE_RECORD_SIZE - 1] == SAUCE_EOF_CHAR) eof_exists = 1;

  // determine if there is a comment and if so, if there is an eof before the comment
  uint8_t totalLines = ((SAUCE*)(&buffer[n - SAUCE_RECORD_SIZE]))->Comments;
  int comment_exists = 1; // first assume comment exists
  if (totalLines == 0) {
    comment_exists = 0;
  } else {
    // comment could already exist
    eof_exists = 0;
    
    // look for comment
    if (n < SAUCE_TOTAL_SIZE(totalLines)) {
      comment_exists = 0;
    } else if (memcmp(&buffer[n - SAUCE_TOTAL_SIZE(totalLines)], SAUCE_COMMENT_ID, 5) != 0) {
      comment_exists = 0;
    }

    // look for eof char
    if (comment_exists && n > SAUCE_TOTAL_SIZE(totalLines)) {
      if (buffer[n - SAUCE_TOTAL_SIZE(totalLines) - 1] == SAUCE_EOF_CHAR) eof_exists = 1;
    }
  }

  // determine where to write
  uint32_t write_index = 0;
  if (comment_exists)
    write_index = (eof_exists) ? n - SAUCE_TOTAL_SIZE(totalLines) - 1 : n - SAUCE_TOTAL_SIZE(totalLines);
  else
    write_index = (eof_exists) ? n - SAUCE_RECORD_SIZE - 1 : n - SAUCE_RECORD_SIZE;

  // move record
  memmove(&buffer[write_index+1+SAUCE_COMMENT_BLOCK_SIZE(lines)], &buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);

  // add eof
  buffer[write_index] = SAUCE_EOF_CHAR;  

  // write comment
  memcpy(&buffer[write_index+1], SAUCE_COMMENT_ID, 5);
  memcpy(&buffer[write_index+6], comment, SAUCE_COMMENT_STRING_LENGTH(lines));

  // update comments field
  ((SAUCE*)(&buffer[write_index+1+SAUCE_COMMENT_BLOCK_SIZE(lines)]))->Comments = lines;

  return write_index + 1 + SAUCE_TOTAL_SIZE(lines);
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
  if (filepath == NULL) {
    SAUCE_SET_ERROR("Filepath was NULL");
    return SAUCE_ENULL;
  }

  SAUCEInfo info;
  uint32_t filesize;
  int res = SAUCE_file_get_info(filepath, &info, &filesize, NULL);
  if (res < 0 && !info.record_exists) return res;

  if (info.eof_exists) info.sauce_length++;
  res = SAUCE_file_truncate(filepath, filesize, info.sauce_length, NULL);
  if (res < 0) return res;
  return 0;
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
  SAUCEInfo info;
  uint32_t filesize;
  char* buffer = NULL;
  int res = SAUCE_file_get_info(filepath, &info, &filesize, &buffer);
  if (res < 0 && !info.record_exists) return res;

  // check if comment doesn't exist
  if (!info.comment_exists) {
    free(buffer);
    if (info.lines == 0) {
      SAUCE_SET_ERROR("%s contains zero comment lines, so no comment can be removed", filepath);
    }
    return SAUCE_ECMISS;
  }

  // copy record
  char record[SAUCE_RECORD_SIZE];
  memcpy(record, &buffer[info.sauce_length - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);
  free(buffer);
  ((SAUCE*)record)->Comments = 0;

  // prep file for writing
  FILE* file;
  res = SAUCE_file_truncate(filepath, filesize, info.sauce_length, &file);
  if (res < 0) return res;

  // write an eof character if needed
  size_t write;
  if (!info.eof_exists) {
    char eof_char = SAUCE_EOF_CHAR;
    write = fwrite(&eof_char, 1, 1, file);
    if (write != 1) {
      fclose(file);
      free(buffer);
      SAUCE_SET_ERROR("Failed to write eof character to %s", filepath);
      return SAUCE_EFFAIL;
    }
  }

  // write buffer to file
  res = fwrite(record, 1, SAUCE_RECORD_SIZE, file);
  fclose(file);
  if (res != SAUCE_RECORD_SIZE) {
    SAUCE_SET_ERROR("Failed to write updated record to %s", filepath);
    return SAUCE_EFFAIL;
  }

  return 0;
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
int SAUCE_remove(char* buffer, uint32_t n) {
  SAUCEInfo info;
  int res = SAUCE_buffer_get_info(buffer, n, &info);
  if (res < 0 && !info.record_exists) return res;

  // erase SAUCE data
  memset(&buffer[info.start], 0, info.sauce_length);
  if (info.eof_exists) {
    buffer[info.start - 1] = 0;
    return n - info.sauce_length - 1;
  }
  return n - info.sauce_length;
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
int SAUCE_Comment_remove(char* buffer, uint32_t n) {
  SAUCEInfo info;
  int res = SAUCE_buffer_get_info(buffer, n, &info);
  if (res < 0 && !info.record_exists) return res;

  // check info on comment
  if (info.lines > 0 && !info.comment_exists) {
    return SAUCE_ECMISS;
  } else if (!info.comment_exists) {
    SAUCE_SET_ERROR("Buffer contains zero comment lines, so no comment can be removed");
    return SAUCE_ECMISS;
  }
  
  // move record in place of comment
  if (info.eof_exists) {
    memmove(&buffer[info.start], &buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);
    ((SAUCE*)(&buffer[info.start]))->Comments = 0;
    return n - SAUCE_COMMENT_BLOCK_SIZE(info.lines);
  }

  // move record in place of comment and add eof
  buffer[info.start] = SAUCE_EOF_CHAR;
  memmove(&buffer[info.start+1], &buffer[n - SAUCE_RECORD_SIZE], SAUCE_RECORD_SIZE);
  ((SAUCE*)(&buffer[info.start+1]))->Comments = 0;
  return n - SAUCE_COMMENT_BLOCK_SIZE(info.lines) + 1;
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
  SAUCEInfo info;
  int res = SAUCE_file_get_info(filepath, &info, NULL, NULL);
  if (res < 0) return 0;
  return 1;
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
  SAUCEInfo info;
  int res = SAUCE_buffer_get_info(buffer, n, &info);
  if (res < 0) return 0;
  return 1;
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
 * @brief Determine if two SAUCE comments are equal. 
 * 
 * 
 *        Both comments must be at least `SAUCE_COMMENT_STRING_LENGTH(lines)` bytes long. 
 *        Anything beyond the given number of lines, including any terminating null 
 *        characters after the last line, will be not compared or read.
 * 
 * @param first the first comment
 * @param second the second comment
 * @param lines the maximum number of lines to compare
 * @return 1 (i.e. true) if the comments are equal; 0 (i.e. false) if the comments are not equal
 */
int SAUCE_Comment_equal(const char* first_comment, const char* second_comment, uint8_t lines) {
  return memcmp(first_comment, second_comment, SAUCE_COMMENT_STRING_LENGTH(lines)) == 0;
}