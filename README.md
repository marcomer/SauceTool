# SAUCE Utility

## Table of Contents
1. [What is SAUCE?](#what-is-sauce)
2. Core Ideas
    - [Summary](#summary)
        - [What counts as a Record or a Comment?](#what-counts-as-a-record-or-a-comment)
        - [What functions can I use?](#what-functions-can-i-use)
    - [Assumptions To Keep In Mind](#assumptions-to-keep-in-mind)
    - [The EOF Character](#the-eof-character)
    - SAUCE Requirements
        - [SAUCE Record Requirements](#sauce-record-requirements)
        - [CommentBlock Requirements](#commentblock-requirements)
    - [Limitations](#limitations)
3. Usage
    - [Data Structures](#data-structures)
    - [Reading](#reading)
    - [Writing](#writing)
    - [Removing](#removing)
    - [Performing Checks](#performing-checks)
    - [Constants](#constants)
    - [Helper Functions](#helper-functions)
    - [Error Codes](#error-codes)


## What is SAUCE?
SAUCE, the Standard Architecture for Universal Comment Extensions, is a protocol for attaching meta data or comments to files. It is mainly intended for ANSI art files, but it can used for many different file types. 

SAUCE has 4 parts: The original file contents, an End-Of-File or [EOF character](#the-eof-character) (Decimal 26, Hex 1A, Ctrl+Z), an optional comment block and the SAUCE record. A SAUCE record should make up the last 128 bytes of a files.

You can see the [official specification](https://www.acid.org/info/sauce/sauce.htm) by Olivier "Tasmaniac" Reubens / ACiD for more information on interpreting SAUCE records.


## Summary

The intention of this library is to offer a way to read, write, remove, and check the correctness of SAUCE records and comment blocks from files and buffer arrays. 

### What counts as a Record or a Comment?
Any checks for SAUCE data correctness strictly follow the [requirements](#sauce-record-requirements) listed further below. Checking or validating any of the fields which are marked as *not required* by the [SAUCE Layout table](https://www.acid.org/info/sauce/sauce.htm), with the exception of the `Comments` field, are beyond the scope of this project.

The **read**, **write**, and **remove** functions will not perform checks for correctness on records or comments. In order to find records or comments, all that will be checked for are the `SAUCE` and `COMNT` IDs and the record's `Comments` field. If the IDs or `Comments` field are incorrect, then the record/comment is considered missing or invalid. 


### What functions can I use?
There are 2 distinct sets of functions for **files** and for **buffer** arrays. Functions that access **files** follow a similiar naming convention to the C std I/O File library (e.g. `SAUCE_fread()`, `SAUCE_Comment_fwrite()`, etc.). Functions that access **buffers** have similiar names but are missing the `f` character (e.g. `SAUCE_read()`, `SAUCE_Comment_write()`, etc.).

The file functions are the most convenient and are adequate for most cases. However, if frequently reopening files is a concern for you or would be impractical, the buffer functions are your solution.

See the Usage section in the [Table of Contents](#table-of-contents) for info on how to use this library.


## Assumptions To Keep In Mind
1. SAUCE is made up of two data structures: (1) a SAUCE record, and (2) an *optional* SAUCE CommentBlock. This library operates on this core assumption. 
2. A file/buffer contains correct SAUCE data **only if** the data adheres to the [requirements](#sauce-record-requirements) listed below.
3. SAUCE CommentBlocks are *optional*. That means comment blocks **cannot** be read, written, replaced, or removed unless a corresponding SAUCE record also exists.
4. Buffer functions require a buffer's length, which is often the parameter `n`. Note that `n` isn't the *actual* size of the allocated array, but the length of the file contents present in the buffer. All buffer functions will treat data from index `0` to `n-1` as the provided file contents. If you are attempting to read, replace, or remove a SAUCE record/comment block, bytes `n-1` to `n-128` must contain the SAUCE record.
5. If you are using the buffer functions, it is your responsibility to make sure your buffer array is large enough to hold your file contents, an EOF character, an optional comment block, and a SAUCE record. 
6. Unexpected behavior may occur if your file/buffer contains invalid, misplaced, or otherwise non-standard SAUCE records/comments.



## The EOF Character
SAUCE specifies that an EOF character (Decimal 26, Hex 1A, Ctrl+Z) should be placed immediately before the SAUCE data as a way to prevent a text/file viewer from reading the SAUCE data as text.

Although EOF characters are important for text/file viewers, this library specifies that an EOF char is *not* required to **read** or **perform checks** on any SAUCE information. Refer to [What counts as a Record or a Comment?](#what-counts-as-a-record-or-a-comment) for what is required for reading.

The **write** functions will attempt to add an EOF character. An EOF character will only be added if the function is *certain* that an EOF does not already exist. The **remove** functions could also add an EOF, but only if you are removing a comment *and* if no EOF character already exists.

The **remove** functions may remove an EOF character if one exists immediately before the SAUCE data. If you are only removing a comment and an EOF character exists, the EOF will *not* be removed.


## SAUCE Record Requirements
A SAUCE record must have the following attributes:
- It must make up the last 128 bytes of a file.
- If there is a CommentBlock, the record must be immediately after the CommentBlock.
- The DataType and FileType fields must be a valid pair.

A SAUCE record must have the following fields:
- **ID** - must be equal to "SAUCE"
- **Version** - must be equal to "00"
- **DataType** - must be a valid SAUCE DataType (see [specs](https://www.acid.org/info/sauce/sauce.htm))
- **FileType** - must be a valid SAUCE FileType (see [specs](https://www.acid.org/info/sauce/sauce.htm))
- **Comments** - must be set to the number of lines contained in the optional SAUCE comment block. 0 indicates no comment block is present



## CommentBlock Requirements
The CommentBlock must have the following attributes:
- The block's **ID field** must be equal to "COMNT".
- The number of lines must be equal to the **Comments** field in the corresponding SAUCE record.
- Each line must be 64 bytes long.
- The CommentBlock must be immediately before the SAUCE record.



## Limitations
This library is designed for common x86 operating systems (i.e. Windows, MacOS, and Linux), so exotic systems may have unexpected behavior. This library relies on the C standard library and POSIX functions.

Currently, files over 2GB are not supported. 

Checking or validating any of the SAUCE record fields which are marked as *not required* by the [SAUCE Layout table](https://www.acid.org/info/sauce/sauce.htm), with the exception of the `Comments` field, are beyond the scope of this project.

Remember, unexpected behavior may occur if your file/buffer contains invalid, misplaced, or otherwise non-standard SAUCE records/comments.

## Data Structures

There are two data structures used to represent SAUCE records/comments. The `SAUCE` struct and the `SAUCE_CommentBlock` struct.


### `SAUCE` struct
A struct that represents a SAUCE record. For more information on each field, see the SAUCE Layout table in the [offical specification](https://www.acid.org/info/sauce/sauce.htm).

```C
  typedef struct SAUCE {
    char          ID[5];
    char          Version[2];
    char          Title[35]; 
    char          Author[20];
    char          Group[20];
    char          Date[8];
    uint32_t      FileSize;
    uint8_t       DataType;
    uint8_t       FileType;
    uint16_t      TInfo1;
    uint16_t      TInfo2;
    uint16_t      TInfo3;
    uint16_t      TInfo4;
    uint8_t       Comments;
    uint8_t       TFlags;
    char          TInfoS[22];
  } SAUCE;
```

### `SAUCE_CommentBlock` struct
A struct representing a SAUCE CommentBlock with additional information about the block.

```C
typedef struct SAUCE_CommentBlock {
  char      ID[5];
  uint8_t   lines;
  char*     comment;
} SAUCE_CommentBlock;
```
The field descriptions are:
- **ID** - The ID of the CommentBlock, should be "COMNT".
- **lines** - The number of comment lines present.
- **comment** - A null-terminated string containing appended lines from the SAUCE comment block.


## Reading
Functions are provided to find and read SAUCE records and CommentBlocks from files/buffers. 

**NOTE**: It is recommended that you first read the SAUCE record to determine how many comment lines exist *before* you attempt to read the comment. However, if you only care for the full comment, attempting to read 255 lines of the comment is guaranteed to give you all available lines.


### Functions
#### `SAUCE_fread(const char* filepath, SAUCE* sauce)`
- From a file, read a SAUCE record into `sauce`.


#### `SAUCE_Comment_fread(const char* filepath, SAUCE_CommentBlock* block, uint8_t nLines)`
- From a file, read at most `nLines` of a SAUCE CommentBlock into `block`. If the file does not contain a comment or the actual number of lines is less than `nLines`, then expect 0 lines or all lines to be read, respectively.


#### `SAUCE_read(const char* buffer, uint32_t n, SAUCE* sauce)`
- From the first `n` bytes of a buffer, read a SAUCE record into `sauce`.


#### `SAUCE_Comment_read(const char* buffer, uint32_t n, SAUCE_CommentBlock* block, uint8_t nLines)`
- From the first `n` bytes of a buffer, read at most `nLines` of a SAUCE CommentBlock into `block`. If the buffer does not contain a comment or the actual number of lines is less than `nLines`, then expect 0 lines or all lines to be read, respectively.


### Return Values
On success, `SAUCE_fread()` and `SAUCE_read()` will return 0. On an error, all SAUCE record read functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

On success, `SAUCE_Comment_fread()` and `SAUCE_Comment_read()` will return the number of lines read. On an error, they will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

**NOTE**: *Each* read function will return an error if the file or buffer are missing a SAUCE record. `SAUCE_fread()` and `SAUCE_read()` ignore SAUCE CommentBlocks and will therefore *not* return an error if a CommentBlock is invalid, meaning the record's "Comments" field was incorrect and the COMNT id could not be found.




## Writing
The write functions can be used to **write** new SAUCE records/CommentBlocks or **replace** existing records/CommentBlocks.

**NOTE**: if a write function returns an error, the file/buffer will not be altered.

**NOTE**: When writing a SAUCE record, the original "Comments" field will always remain unchanged. The "Comments" field is only updated when a CommentBlock is successfully written or removed. If there is no existing SAUCE record, then the "Comments" field will always be set to 0. 

### Functions
#### `SAUCE_fwrite(const char* filepath, const SAUCE* sauce)`
- Write a SAUCE record to a file.
- If the file already contains a SAUCE record, the record will be replaced.
- An EOF character will be added if the file previously did not contain a SAUCE record.

#### `SAUCE_Comment_fwrite(const char* filepath, const char* comment)`
- Write a SAUCE CommentBlock to a file, replacing a CommentBlock if one already exists.
- `comment` must be null-terminated.
- The "Comments" field of the file's SAUCE record will be updated to the new number of comment lines.
- **NOTE**: The null character of the comment string is not included in the written SAUCE CommentBlock. The comment's last line will be padded with spaces if the comment can't fill the entire line.

#### `SAUCE_write(char* buffer, uint32_t n, const SAUCE* sauce)`
- Write a SAUCE record to a buffer.
- If the last 128 bytes of the buffer (bytes `n-1` to `n-128`) contain a SAUCE record, the buffer's SAUCE record will be replaced. Otherwise, the EOF character and the new SAUCE record will be appended to the buffer at index `n`.
- **Important**: To prevent a buffer overflow error when appending a new record, the buffer's actual size must be at least `n` + 129 bytes (the size of a SAUCE record including an EOF character).

#### `SAUCE_Comment_write(const char* buffer, uint32_t n, const char* comment)`
- Write a SAUCE CommentBlock to a buffer, replacing a CommentBlock if one already exists.
- `comment` must be null-terminated.
- If the last 128 bytes of the buffer (bytes `n-1` to `n-128`) contain a SAUCE record, the CommentBlock will be written. Otherwise, an error will be returned.
- The "Comments" field of the buffer's SAUCE record will be updated to the new number of comment lines.
- **Important**: To prevent a buffer overflow error when writing a new comment, the buffer's actual size must be at least `n` + `SAUCE_COMMENT_BLOCK_SIZE(number of comment lines)`.
- **NOTE**: The null character of the comment string is not included in the written SAUCE CommentBlock. The comment's last line will be padded with spaces if the comment can't fill the entire line.


### Return Values
On success, all **file** write functions will return 0. On error, all **file** write functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

On success, all **buffer** write functions will return the new length of the buffer. On error, all **buffer** write functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.




## Removing
Functions are provided to remove SAUCE records and CommentBlocks from files/buffers.

**NOTE**: if a function fails to remove a SAUCE record/comment, the file/buffer will not be altered.


### Functions
#### `SAUCE_fremove(const char* filepath)`
- Remove a SAUCE record from a file, along with the SAUCE CommentBlock if one exists.
- The EOF character will be removed as well.

#### `SAUCE_Comment_fremove(const char* filepath)`
- Remove a SAUCE CommentBlock from a file.
- The "Comments" field of the file's SAUCE record will be set to 0.

#### `SAUCE_remove(const char* buffer, uint32_t n)`
- Remove a SAUCE Record from the first `n` bytes of a buffer, along with the SAUCE CommentBlock if one exists.
- The EOF character will be removed as well.

#### `SAUCE_Comment_remove(const char* buffer, uint32_t n)`
- Remove a SAUCE CommentBlock from the first `n` bytes of a buffer.
- The "Comments" field of the buffer's SAUCE record will be set to 0.


### Return Values
On success, all **file** remove functions will return 0. On error, all **file** remove functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

On success, all **buffer** remove functions will return the new length of the buffer. On error, all **buffer** remove functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.




## Performing Checks
`SAUCE_check_file()` and `SAUCE_check_buffer()` are provided to check the correctness of SAUCE data in a file/buffer. SAUCE data is deemed correct if it exists and adheres to the SAUCE record and CommentBlock [requirements](#sauce-record-requirements).

`SAUCE_equal()` and `SAUCE_Comment_equal()` are provided to check if two SAUCE structs or two SAUCE CommentBlocks are equal.

### Functions
#### `SAUCE_check_file(const char* filepath)`
- Check if a file contains correct SAUCE data. 
- This will check the SAUCE data against the SAUCE record and CommentBlock [requirements](#sauce-record-requirements).

#### `SAUCE_check_buffer(const char* buffer, uint32_t n)`
- Check if the first `n` bytes of a buffer contain correct SAUCE data.
- This will check the SAUCE data against the SAUCE record and CommentBlock [requirements](#sauce-record-requirements).

#### `SAUCE_equal(const SAUCE* first, const SAUCE* second)`
- Determine if two SAUCE records are equal.
- SAUCE records are equal if each field between the SAUCE records match.

#### `SAUCE_Comment_equal(const SAUCE_CommentBlock* first, const SAUCE_CommentBlock* second)`
- Determine if two SAUCE_CommentBlocks are equal.
- SAUCE_CommentBlocks are equal if the content of each field match between the CommentBlocks.

### Return Values

On success, `SAUCE_check_file()` and `SAUCE_check_buffer()` will return 1 (i.e. true) if the file/buffer contained correct SAUCE data. On error, meaning that no SAUCE data existed or the SAUCE data was incorrect, the check functions will return 0 (i.e. false). If 0 is returned, you can call `SAUCE_get_error()` to learn more about why the check failed.

The `SAUCE_equal()` and `SAUCE_Comment_equal()` will return a boolean value: 1 for true, and 0 for false.




## Constants
### `SAUCE_COMMENT_LINE_LENGTH`
Length of a single Comment Line in bytes

### `SAUCE_RECORD_SIZE`
The size of a SAUCE record in bytes

### `SAUCE_DataType` enum 
An enum to help with identifying DataTypes. All DataType constants start with `SAUCE_DT_` and are named according to the DataTypes listed in the [specs](https://www.acid.org/info/sauce/sauce.htm).

### `SAUCE_FileType` enum
An enum to help with identifying FileTypes. All FileType constants start with `SAUCE_FT_` and are named according to the FileTypes listed in the [specs FileType table](https://www.acid.org/info/sauce/sauce.htm#FileType).




## Helper Functions
### `SAUCE_get_error()`
Get an error message about the last SAUCE error that occurred. An empty
string will be returned if no SAUCE error has yet to occur.

### `SAUCE_set_default(SAUCE* sauce)`
Fill a SAUCE struct with the default fields. ID and Version fields will be set
to their required values. All other fields will be set to their defaults, which
is typically 0 or spaces.

### `SAUCE_num_lines(const char* string)`
Determine how many comment lines a string will need in order to place it in a CommentBlock.

### `SAUCE_COMMENT_BLOCK_SIZE(lines)`
Macro function that determines how large an actual CommentBlock will be in bytes according to the number of lines present. This includes the 5 bytes for the COMNT id.

### `SAUCE_COMMENT_STRING_LENGTH(lines)`
Macro function that determines the length a comment string, not including a terminating null character. This does *not* include the 5 bytes for a COMNT id, since that is not included in a comment string.

### `SAUCE_TOTAL_SIZE(lines)`
Macro function that determines how large a record and optional comment will be in bytes according to the number of lines present. 


## Error Codes
Most functions will return an error code if an error occurs. Remember that you can call `SAUCE_get_error()` to learn more about an error that occurred.
- `SAUCE_EFOPEN` - Could not open a file
- `SAUCE_ERMISS` - SAUCE record could not be found
- `SAUCE_ECMISS` - SAUCE CommentBlock could not be found
- `SAUCE_ESHORT` - The given file/buffer was too short to contain a record
- `SAUCE_ENULL` - A given pointer was NULL
- `SAUCE_EFFAIL` - A file operation failed
- `SAUCE_EEMPTY` - The file was empty