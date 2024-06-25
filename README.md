# SAUCE Utility

## Table of Contents
1. [What is SAUCE?](#what-is-sauce)
2. Core Ideas
    - [Summary](#summary)
    - [Assumptions To Keep In Mind](#assumptions-to-keep-in-mind)
    - SAUCE Requirements
        - [SAUCE Record Requirements](#sauce-record-requirements)
        - [CommentBlock Requirements](#commentblock-requirements)
3. Usage
    - [Data Structures](#data-structures)
    - [Reading](#reading)
    - [Writing](#writing)
    - [Removing](#removing)
    - [Performing Checks](#performing-checks)
    - [Macros and Constants](#macros-and-constants)


## What is SAUCE?
SAUCE, the Standard Architecture for Universal Comment Extensions, is a protocol for attaching meta data or comments to files. It is mainly intended for ANSI art files, but it can used for many different file types. 

SAUCE has 4 parts: The original file contents, an End-Of-File or EOF character (Decimal 26, Hex 1A, Ctrl+Z), an optional comment block and the SAUCE record. A SAUCE record should make up the last 128 bytes of a files.

You can see the [official specification](https://www.acid.org/info/sauce/sauce.htm) by Olivier "Tasmaniac" Reubens / ACiD for more information on interpreting SAUCE records.


## Summary

The intention of this library is to offer a way to read, write, replace, remove, and check SAUCE records and comment blocks from files and buffer arrays. 

Any checks for SAUCE data correctness strictly follow the [requirements](#sauce-record-requirements) listed further below. Checking or validating any of the fields which are marked as *not required* by the [SAUCE Layout table](https://www.acid.org/info/sauce/sauce.htm), with the exception of the `Comments` field, are beyond the scope of this project.

This library provides functions to access and modify files and buffer arrays of bytes. The file functions are the most convenient and are adequate for most cases. However, if frequently reopening files is a concern for you or would be impractical, the buffer functions are your solution.

There are 2 distinct sets of functions for **files** and for **buffer** arrays. Functions that access **files** follow a similiar naming convention to the C std I/O File library (e.g. `SAUCE_fread()`, `SAUCE_Comment_fwrite()`, etc.). Functions that access **buffers** have similiar names but are missing the `f` character (e.g. `SAUCE_read()`, `SAUCE_Comment_write()`, etc.).

You can refer to the [Table of Contents](#table-of-contents) to navigate to the relevant documentation section.


## Assumptions To Keep In Mind
1. SAUCE is made up of two data structures: (1) a SAUCE record, and (2) an *optional* SAUCE CommentBlock. This library operates on this core assumption. 
2. A file/buffer contains correct SAUCE data **only if** the data adheres to the [requirements](#sauce-record-requirements) listed below.
3. SAUCE CommentBlocks are *optional*. That means comment blocks **cannot** be read, written, replaced, or removed unless a corresponding SAUCE record also exists.
4. Buffer functions require a buffer's length, which is often the parameter `n`. Note that `n` isn't the *actual* size of the allocated array, but the length of the file contents present in the buffer. All buffer functions will treat data from index `0` to `n-1` as the provided file contents. If you are attempting to read, replace, or remove a SAUCE record/comment block, bytes `n-1` to `n-128` must contain the SAUCE record.
5. If you are using the buffer functions, it is your responsibility to make sure your buffer array is large enough to hold your file contents, an EOF character, an optional comment block, and a SAUCE record. 
6. Unexpected behavior may occur if your file/buffer contains invalid, misplaced, or otherwise non-standard SAUCE records/comments.


## SAUCE Record Requirements
A SAUCE record must have the following attributes:
- It must be 128 bytes large.
- It must make up the last 128 bytes of a file.
- There must be an EOF character immediately before the record. If there is a CommentBlock, the EOF character must be immediately before the CommentBlock.
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
- There must be an EOF character immediately before the CommentBlock.
- The CommentBlock must be immediately before the SAUCE record.


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

Note that because CommentBlocks are *optional*, you must read the SAUCE record first in order to determine if a CommentBlock exists, and if so, how many comment lines can be read.


### Functions

#### `SAUCE_fread(const char* filepath, const SAUCE* sauce)`
- From a file, read a SAUCE record into `sauce`.


#### `SAUCE_Comment_fread(const char* filepath, const SAUCE_CommentBlock* block, uint8_t nLines)`
- From a file, read `nLines` lines of a SAUCE CommentBlock into `block`.


#### `SAUCE_read(const char* buffer, uint32_t n, const SAUCE* sauce)`
- From a buffer containing file contents in the first `n` bytes, read a SAUCE record into `sauce`.


#### `SAUCE_Comment_read(const char* buffer, uint32_t n, const SAUCE_CommentBlock* block, uint8_t nLines)`
- From a buffer containing file contents in the first `n` bytes, read `nLines` lines of a SAUCE CommentBlock into `block`.


### Return Values

On success, all read function will return 0. On an error, all read functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

*Each* read function will return an error if the file or buffer are missing a SAUCE record. `SAUCE_fread()` and `SAUCE_read()` ignore SAUCE CommentBlocks and will therefore *not* return an error if a CommentBlock is missing. `SAUCE_Comment_fread()` and `SAUCE_Comment_read()` will return an error if the comment block is missing.

Error codes that can be returned by read functions include:
- `SAUCE_EFOPEN` - Could not open file
- `SAUCE_ERMISS` - SAUCE record is missing
- `SAUCE_ECMISS` - SAUCE CommentBlock is missing (only ever returned by the Comment read functions)
- `SAUCE_EINCOMP` - SAUCE is incomplete, meaning only part of it exists
- `SAUCE_EBUFSHORT` - The given buffer length is too short
- `SAUCE_EFFAIL` - A file read operation failed



## Writing
The write functions can be used to **write** new SAUCE records/CommentBlocks or **replace** existing records/CommentBlocks. Replacing records/CommentBlocks can be more involved than simply appending a new record, so see the Details section for more info on replacing existing records/CommentBlocks.

Note that if a write function returns an error, the file/buffer will not be altered.

When writing a SAUCE record, the original "Comments" field will always remain unchanged. The "Comments" field is only updated when a CommentBlock is successfully written or removed. If there is no existing SAUCE record, then the "Comments" field will always be set to 0. 

### Functions
#### `SAUCE_fwrite(const char* filepath, const SAUCE* sauce)`
- Write a SAUCE record to a file.
- If the file already contains a SAUCE record, the record will be replaced.

#### `SAUCE_Comment_fwrite(const char* filepath, const char* comment)`
- Write a SAUCE CommentBlock to a file, replacing a CommentBlock if one already exists.
- `comment` must be null-terminated.
- If the file already contains a CommentBlock, the block will be replaced.
- The "Comments" field of the file's SAUCE record will be updated to the new number comment lines.

#### `SAUCE_write(const char* buffer, uint32_t n, const SAUCE* sauce)`
- Write a SAUCE record to a buffer.
- The first `n` bytes of the buffer are treated as the file contents.
- If bytes `n-1` to `n-128` (the last 128 bytes of the buffer) contain a SAUCE record, the buffer's SAUCE record will be replaced. Otherwise, an EOF character and the new SAUCE record will be appended to the buffer at index `n`.
- **Important**: To prevent a buffer overflow error when appending a new record, the buffer's actual size must be at least `n` + 129 bytes (the size of a SAUCE record including an EOF character).

#### `SAUCE_Comment_write(const char* buffer, uint32_t n, const char* comment)`
- Write a SAUCE CommentBlock to a buffer, replacing a CommentBlock if one already exists.
- `comment` must be null-terminated.
- The first `n` bytes of the buffer are are treated as the file contents.
- If bytes `n-1` to `n-128` (the last 128 bytes of the buffer) contain a SAUCE record, the block will be written. Otherwise, an error will be returned.
- The "Comments" field of the buffer's SAUCE record will be updated to the new number comment lines.
- **Important**: To prevent a buffer overflow error, the buffer's actual size must be at least the size of the contained file contents + `SAUCE_COMMENT_BLOCK_SIZE(nLines)` + 129 bytes (the size of a SAUCE record including an EOF character).


### Return Values
On success, all **file** write functions will return 0. On error, all **file** write functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

On success, all **buffer** write functions will return the new length of the buffer. On error, all **buffer** write functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

Error codes that can be returned by write functions include:
- `SAUCE_EFOPEN` - Could not open file
- `SAUCE_ERMISS` - SAUCE record is missing
- `SAUCE_EINCOMP` - SAUCE is incomplete, meaning only part of it exists
- `SAUCE_EBUFSHORT` - The given buffer length is too short
- `SAUCE_EFFAIL` - A file read/write operation failed



## Removing
Functions are provided to remove SAUCE records and CommentBlocks from files/buffers.

Note that if a function fails to remove a SAUCE record/comment, the file/buffer will not be altered.


### Functions
#### `SAUCE_fremove(const char* filepath)`
- Remove a SAUCE record from a file, along with the SAUCE CommentBlock if one exists.
- The EOF character will be removed as well.

#### `SAUCE_Comment_fremove(const char* filepath)`
- Remove a SAUCE CommentBlock from a file.
- The "Comments" field of the file's SAUCE record will be set to 0.

#### `SAUCE_remove(const char* buffer, uint32_t n)`
- Remove a SAUCE Record from a buffer, along with the SAUCE CommentBlock if one exists.
- The first `n` bytes of the buffer are are treated as the file contents.

#### `SAUCE_Comment_remove(const char* buffer, uint32_t n)`
- Remove a SAUCE CommentBlock from a buffer.
- The first `n` bytes of the buffer are are treated as the file contents.
- The "Comments" field of the buffer's SAUCE record will be set to 0.


### Return Values
On success, all **file** remove functions will return 0. On error, all **file** remove functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

On success, all **buffer** remove functions will return the new length of the buffer. On error, all **buffer** remove functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

Error codes that can be returned by remove functions include:
- `SAUCE_EFOPEN` - Could not open file
- `SAUCE_ERMISS` - SAUCE record could not be found
- `SAUCE_ECMISS` - SAUCE CommentBlock could not be found
- `SAUCE_EBUFSHORT` - The given buffer length is too short
- `SAUCE_EFFAIL` - A file read/write operation failed


## Performing Checks
Functions are provided to check if a file/buffer contains correct SAUCE data that adheres to the SAUCE specification. SAUCE data is considered correct if the end of a file/buffer contains an EOF character, an *optional* CommentBlock, and a SAUCE record. Since the CommentBlock is optional, the CommentBlock will only be checked for correctness if the corresponding SAUCE record's "Comments" field is greater than 0.


### Functions
#### `SAUCE_check_file(const char* filepath)`
- Check if a file has a SAUCE record with the correct, required fields (refer to Required Fields list).
- If the SAUCE record claims that a CommentBlock exists, then the CommentBlock will also be checked.

#### `SAUCE_check_buffer(const char* buffer, uint32_t n)`
- Check if a buffer has a SAUCE record with the correct, required fields (refer to Required Fields list).
- The first `n` bytes of the buffer are are treated as the file contents.
- If the SAUCE record claims that a CommentBlock exists, then the CommentBlock will also be checked.


### Return Values
The two functions above will return a 1 (true) if the file/buffer contains correct SAUCE data. If the file/buffer does **not** contain correct SAUCE data, then the two functions above will return a 0 (false). If 0 is returned, you can call `SAUCE_get_error()` to learn more about why the check failed.


## Macros and Constants
### Macros
- `SAUCE_COMMENT_LINE_LENGTH` - Length of a single Comment Line.
- `SAUCE_RECORD_SIZE` - Size of a single SAUCE record in bytes.
- `SAUCE_COMMENT_BLOCK_SIZE(lines)` - Function that determines how large a CommentBlock will be in bytes according to the number of lines present.


### `SAUCE_DataType` enum 
An enum to help with identifying DataTypes. All constants start with `SAUCE_DT_` and are named according to the DataTypes listed in the [specs](https://www.acid.org/info/sauce/sauce.htm).


### `SAUCE_FileType` enum
An enum to help with identifying FileTypes. All constants start with `SAUCE_FT_` and are named according to the FileTypes listed in the [specs FileType table](https://www.acid.org/info/sauce/sauce.htm#FileType).