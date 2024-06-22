#ifndef SAUCE_PARSE_HEADER_INCLUDED
#define SAUCE_PARSE_HEADER_INCLUDED

#include <stdint.h>
#include <stdio.h>


/*
 * Official SAUCE specification can be found at:
 * https://www.acid.org/info/sauce/sauce.htm
 */ 


/**
 * @brief SAUCE DataType mapping. Refer to SAUCE specification
 *        (https://www.acid.org/info/sauce/sauce.htm#FileType) for details on
 *        determining FileType.
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




// The length of a single Comment Line
#define SAUCE_COMMENT_LINE_LENGTH     64

// The size of a SAUCE record in bytes
#define SAUCE_RECORD_SIZE             128

// Determine how large a comment block will be in bytes according to the number of lines present
#define SAUCE_COMMENT_BLOCK_SIZE(lines)         (lines * SAUCE_COMMENT_LINE_LENGTH + 5)





/**
 * @brief Struct containing SAUCE record data.
 * 
 */
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



/**
 * @brief Struct containing a SAUCE Comment Block and additional information on the length of the comment.
 * 
 */
typedef struct SAUCE_CommentBlock {
  char      ID[5];        // SAUCE comment block indentification. This should be equal to "COMNT".
  uint16_t  length;       // String length of the comment
  char*     comment;      // A null-terminated comment string containing appended lines from the SAUCE comment block 
} SAUCE_CommentBlock;









/**
 * @brief Get an error message about the last SAUCE error that occurred.
 * 
 * @return an error message, or an empty string if no SAUCE error has occurred
 */
const char* SAUCE_get_error(void);



/**
 * @brief Fill a SAUCE struct with a default (i.e. empty) SAUCE record.
 * 
 * @param sauce SAUCE struct
 */
void SAUCE_fill_default(const SAUCE* sauce);





/**
 * @brief From a file, read a SAUCE record.
 * 
 * @param filepath a path to a file 
 * @param sauce a SAUCE struct that will be filled with the parsed SAUCE info
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_fread(const char* filepath, const SAUCE* sauce);


/**
 * @brief From a given buffer, read a SAUCE record.
 * 
 * @param buffer pointer to a buffer
 * @param length the length of the buffer
 * @param sauce SAUCE struct that will be filled with the parsed SAUCE info
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_read(const char* buffer, uint32_t length, const SAUCE* sauce);



/**
 * @brief Write a SAUCE record to a file. If the file already contains a SAUCE record, the record will be replaced.
 * 
 * @param filepath a path to a file
 * @param sauce SAUCE struct
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_fwrite(const char* filepath, const SAUCE* sauce);


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
int SAUCE_write(const char* buffer, const SAUCE* sauce);










/**
 * @brief From a file, read a SAUCE CommentBlock.
 * 
 * @param filepath a path to a file 
 * @param block SAUCE_CommentBlock to be filled with the comment
 * @param lines the number of lines to read
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_fread(const char* filepath, const SAUCE_CommentBlock* block, uint8_t lines);


/**
 * @brief From a given buffer, read a SAUCE CommentBlock.
 * 
 * @param buffer pointer to a buffer
 * @param length the length of the buffer
 * @param block SAUCE_CommentBlock to be filled with the comment
 * @param lines the number of lines to read
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_read(const char* buffer, uint32_t length, const SAUCE_CommentBlock* block, uint8_t lines);



/**
 * @brief Add a SAUCE CommentBlock to a file. This will update the Comments field of
 *        the file's SAUCE record with the number of Comment Lines. If the file already contains
 *        a Commentblock, the block will be replaced.
 * 
 * @param filepath a path to a file
 * @param comment comment to be written
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_fwrite_open(const char* filepath, const char* comment);


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
int SAUCE_Comment_write(const char* buffer, const char* comment);








/**
 * @brief Remove a SAUCE record from a file, along with the SAUCE CommentBlock if it exists.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_fremove(const char* filepath);


/**
 * @brief Remove a SAUCE record from a buffer, along with the SAUCE CommentBlock if it exists.
 * 
 * @param buffer pointer to buffer
 * @param endIndex the index immediately after the last byte of data in the buffer
 * @return the new endIndex of the buffer. On error, a negative number is returned
 */
int SAUCE_remove(const char* buffer, uint32_t endIndex);


/**
 * @brief Remove a SAUCE CommentBlock from a file. This will update the Comments field of
 *        the file's SAUCE record to 0.
 * 
 * @param filepath a path to a file
 * @return 0 on success. On error, a negative number is returned
 */
int SAUCE_Comment_fremove(const char* filepath);


/**
 * @brief Remove a SAUCE CommentBlock from a buffer. This will update the Comments field of
 *        the buffer's SAUCE record to 0.
 * 
 * @param buffer pointer to buffer
 * @param endIndex the index immediately after the last byte of data in the buffer
 * @return the new endIndex of the buffer. On error, a negative number is returned
 */
int SAUCE_Comment_remove(const char* buffer, uint32_t endIndex);










#endif //SAUCE_PARSE_HEADER_INCLUDED