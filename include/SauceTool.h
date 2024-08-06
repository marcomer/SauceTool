/**
 * SauceTool
 * Copyright (c) 2024 marcomer
 * This project is licensed under the MIT License.
 */

#ifndef SAUCE_PARSE_HEADER_INCLUDED
#define SAUCE_PARSE_HEADER_INCLUDED
#include <stdint.h>


// Data structures

/**
 * @brief Struct containing SAUCE record data.
 * 
 */
#pragma pack(push, 1)
typedef struct SAUCE {
  char          ID[5];            // SAUCE identification. This should be equal to "SAUCE"
  char          Version[2];       // SAUCE version number, should be "00"
  char          Title[35];        // The title of the file
  char          Author[20];       // The (nick)name or handle of the creater of the file
  char          Group[20];        // The name of the group or company the creator is employed by
  char          Date[8];          // The date the file was created. The format for the date is CCYYMMDD (century, year, month, day). Ex: 4 May 2013 -> 20130504
  uint32_t      FileSize;         // The original file size not including the SAUCE information
  uint8_t       DataType;         // Type of data
  uint8_t       FileType;         // Type of file
  uint16_t      TInfo1;           // Type dependent numeric information field 1
  uint16_t      TInfo2;           // Type dependent numeric information field 2
  uint16_t      TInfo3;           // Type dependent numeric information field 3
  uint16_t      TInfo4;           // Type dependent numeric information field 4
  uint8_t       Comments;         // Number of lines in the extra SAUCE comment block. 0 indicates no comment block is present
  uint8_t       TFlags;           // Type dependent flags 
  char          TInfoS[22];       // Type dependent string information field
} SAUCE;
#pragma pack(pop)




// Constants and Helpful Macros

/*
 * Official SAUCE specification can be found at:
 * https://www.acid.org/info/sauce/sauce.htm
 */ 


/**
 * @brief Enum constants to help with identifying SAUCE DataTypes. 
 *        Refer to SAUCE specification (https://www.acid.org/info/sauce/sauce.htm#FileType) 
 *        for details on determining FileType.
 * 
 */
enum SAUCE_DataType {
  SAUCE_DT_NONE,            // Undefined filetype
  SAUCE_DT_CHARACTER,       // Character based file
  SAUCE_DT_BITMAP,          // Bitmap graphic and animation files
  SAUCE_DT_VECTOR,          // Vector graphic file
  SAUCE_DT_AUDIO,           // Audio file
  SAUCE_DT_BINARYTEXT,      // Raw memory copy of a text mode screen. Also know as a .BIN file. 
  SAUCE_DT_XBIN,            // XBin or eXtended BIN file
  SAUCE_DT_ARCHIVE,         // Archive file
  SAUCE_DT_EXECUTABLE       // Executable file
};


/**
 * @brief Enum constants to help with identifying SAUCE FileTypes. 
 *        Refer to SAUCE specification (https://www.acid.org/info/sauce/sauce.htm#FileType) 
 *        for details on what files are supported and how to parse the TInfo.
 * 
 */
enum SAUCE_FileType {
  SAUCE_FT_NONE = 0,

  SAUCE_FT_ASCII = 0,
  SAUCE_FT_ANSi,
  SAUCE_FT_ANSiMation,
  SAUCE_FT_RIP_script,
  SAUCE_FT_PCBoard,
  SAUCE_FT_Avatar,
  SAUCE_FT_HTML,
  SAUCE_FT_Source,
  SAUCE_FT_TundraDraw,
  SAUCE_FT_MAX_CHARACTER = 8,

  SAUCE_FT_GIF = 0,
  SAUCE_FT_PCX,
  SAUCE_FT_LBM_IFF,
  SAUCE_FT_TGA,
  SAUCE_FT_FLI,
  SAUCE_FT_FLC,
  SAUCE_FT_BMP,
  SAUCE_FT_GL,
  SAUCE_FT_DL,
  SAUCE_FT_WPG_BITMAP,
  SAUCE_FT_PNG,
  SAUCE_FT_JPG,
  SAUCE_FT_MPG,
  SAUCE_FT_AVI,
  SAUCE_FT_MAX_BITMAP = 13,

  SAUCE_FT_DXF = 0,
  SAUCE_FT_DWG,
  SAUCE_FT_WPG_VECTOR,
  SAUCE_FT_3DS,
  SAUCE_FT_MAX_VECTOR = 3,

  SAUCE_FT_MOD = 0,
  SAUCE_FT_669,
  SAUCE_FT_STM,
  SAUCE_FT_S3M,
  SAUCE_FT_MTM,
  SAUCE_FT_FAR,
  SAUCE_FT_ULT,
  SAUCE_FT_AMF,
  SAUCE_FT_DMF,
  SAUCE_FT_OKT,
  SAUCE_FT_ROL,
  SAUCE_FT_CMF,
  SAUCE_FT_MID,
  SAUCE_FT_SADT,
  SAUCE_FT_VOC,
  SAUCE_FT_WAV,
  SAUCE_FT_SMP8,
  SAUCE_FT_SMP8S,
  SAUCE_FT_SMP16,
  SAUCE_FT_SMP16S,
  SAUCE_FT_PATCH8,
  SAUCE_FT_PATCH16,
  SAUCE_FT_XM,
  SAUCE_FT_HSC,
  SAUCE_FT_IT,
  SAUCE_FT_MAX_AUDIO = 24,

  SAUCE_FT_XBIN = 0,

  SAUCE_FT_ZIP = 0,
  SAUCE_FT_ARJ,
  SAUCE_FT_LZH,
  SAUCE_FT_ARC,
  SAUCE_FT_TAR,
  SAUCE_FT_ZOO,
  SAUCE_FT_RAR,
  SAUCE_FT_UC2,
  SAUCE_FT_PAK,
  SAUCE_FT_SQZ,
  SAUCE_FT_MAX_ARCHIVE = 9,

  SAUCE_FT_EXE = 0,
};


// The required value for the SAUCE record ID field
#define SAUCE_RECORD_ID               "SAUCE"

// The required value for the SAUCE CommentBlock ID field
#define SAUCE_COMMENT_ID              "COMNT"

// The EOF character value specified by SAUCE
#define SAUCE_EOF_CHAR                0x1AU

// The length of a single Comment Line
#define SAUCE_COMMENT_LINE_LENGTH     64

// The size of a SAUCE record in bytes
#define SAUCE_RECORD_SIZE             128

// Determine how large a comment block will be in bytes according to the number of lines present.
// This includes the 5 bytes for the COMNT id.
#define SAUCE_COMMENT_BLOCK_SIZE(lines)         (((uint8_t)lines) ? ((uint8_t)lines * (uint16_t)SAUCE_COMMENT_LINE_LENGTH + 5) : 0)

// Determine the length a comment string, not including a terminating null character.
// This does not include the 5 bytes for a COMNT id, since that is not included in a comment string.
#define SAUCE_COMMENT_STRING_LENGTH(lines)      (((uint8_t)lines) ? ((uint8_t)lines * (uint16_t)SAUCE_COMMENT_LINE_LENGTH) : 0)

// Determine how large a record and optional comment 
#define SAUCE_TOTAL_SIZE(lines)                 ((uint16_t)SAUCE_RECORD_SIZE + SAUCE_COMMENT_BLOCK_SIZE(lines))


// Error Codes

#define SAUCE_EFOPEN    -1    // Could not open file
#define SAUCE_ERMISS    -2    // SAUCE record could not be found
#define SAUCE_ECMISS    -3    // SAUCE CommentBlock could not be found
#define SAUCE_ESHORT    -4    // The given file/buffer was too short to contain a record
#define SAUCE_ENULL     -5    // A given pointer was NULL
#define SAUCE_EFFAIL    -6    // A file operation failed
#define SAUCE_EEMPTY    -7    // The file was empty
#define SAUCE_EOTHER    -8    // An error occurred, please call SAUCE_get_error() for latest error message


// Helper Functions

/**
 * @brief Get an error message about the last SAUCE error that occurred.
 * 
 * @return an error message, or an empty string if no SAUCE error has occurred yet
 */
const char* SAUCE_get_error(void);


/**
 * @brief Clear the last error message. Will do nothing if no SAUCE error has occurred yet.
 * 
 */
void SAUCE_clear_error(void);


/**
 * @brief Fill a SAUCE struct with the default fields. ID and Version fields will be set
          to their required values. All other fields will be set to their defaults, which
          is typically 0 or spaces.
 * 
 * @param sauce a SAUCE struct to be set
 */
void SAUCE_set_default(SAUCE* sauce);


/**
 * @brief Determine how many comment lines a string will need in order to
 *        place it in a CommentBlock.
 * 
 * @param string a null-terminated string
 * @return how many lines the string will need
 */
uint8_t SAUCE_num_lines(const char* string);





// Read Functions

/**
 * @brief From a file, read a SAUCE record into `sauce`.
 * 
 * @param filepath a path to a file 
 * @param sauce a SAUCE struct that will be filled with the parsed SAUCE record
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_fread(const char* filepath, SAUCE* sauce);


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
int SAUCE_Comment_fread(const char* filepath, char* comment, uint8_t nLines);


/**
 * @brief From the first `n` bytes of a buffer, read a SAUCE record into `sauce`.
 * 
 * @param buffer pointer to a buffer
 * @param n the length of the buffer
 * @param sauce a SAUCE struct that will be filled with the parsed SAUCE record
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_read(const char* buffer, uint32_t n, SAUCE* sauce);


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
int SAUCE_Comment_read(const char* buffer, uint32_t n, char* comment, uint8_t nLines);





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
int SAUCE_fwrite(const char* filepath, const SAUCE* sauce);


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
int SAUCE_Comment_fwrite(const char* filepath, const char* comment, uint8_t lines);


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
int SAUCE_write(char* buffer, uint32_t n, const SAUCE* sauce);


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
int SAUCE_Comment_write(char* buffer, uint32_t n, const char* comment, uint8_t lines);





// Remove Functions

/**
 * @brief Remove a SAUCE record from a file, along with the SAUCE CommentBlock if one exists.
 *        The EOF character will be removed as well.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_fremove(const char* filepath);


/**
 * @brief Remove a SAUCE CommentBlock from a file. The "Comments" field of the file's SAUCE
 *        record will be set to 0.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative error code is returned. Use `SAUCE_get_error()`
 *         to get more info on the error.
 */
int SAUCE_Comment_fremove(const char* filepath);


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
int SAUCE_remove(char* buffer, uint32_t n);


/**
 * @brief Remove a SAUCE CommentBlock from the first `n` bytes of a buffer.
 *        The "Comments" field of the buffer's SAUCE record will be set to 0.
 * 
 * @param buffer pointer to buffer
 * @param n the length of the buffer
 * @return On success, the new length of the buffer is returned. On error, a negative error code
 *         is returned. Use `SAUCE_get_error()` to get more info on the error.
 */
int SAUCE_Comment_remove(char* buffer, uint32_t n);





// Functions for performing checks

/**
 * @brief Check if a file contains valid SAUCE data. This will check the SAUCE data against
 *        the SAUCE record and CommentBlock requirements listed in the docs.
 * 
 * @param filepath path to a file
 * @return 1 (i.e. true) if the file contains correct SAUCE data; 0 (i.e. false) if the file does not contain any
 *         SAUCE data or if the SAUCE data was incorrect. If 0 is returned, you can call `SAUCE_get_error()` to 
 *         learn more about why the check failed.
 */
int SAUCE_check_file(const char* filepath);


/**
 * @brief Check if the first `n` bytes of a buffer contain correct SAUCE data. This will check
 *        the data against the SAUCE record and CommentBlock requirements listed in the docs.
 * 
 * @param buffer pointer to a buffer
 * @param n the length of the buffer
 * @return 1 (i.e true) if the buffer contains correct SAUCE data; 0 (i.e. false) if the buffer does not contain any
 *         SAUCE data or if the SAUCE data was incorrect. If 0 is returned, you can call `SAUCE_get_error()` to 
 *         learn more about why the check failed.
 */
int SAUCE_check_buffer(const char* buffer, uint32_t n);


/**
 * @brief Determine if two SAUCE records are equal. SAUCE records are equal if
 *        each field between the SAUCE records match.
 * 
 * @param first the first SAUCE struct
 * @param second the second SAUCE struct
 * @return 1 (i.e. true) if the records are equal; 0 (i.e. false) if the records are not equal
 */
int SAUCE_equal(const SAUCE* first, const SAUCE* second);


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
int SAUCE_Comment_equal(const char* first_comment, const char* second_comment, uint8_t lines);


#endif //SAUCE_PARSE_HEADER_INCLUDED