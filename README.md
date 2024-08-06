# SauceTool
The intention of this library is to offer a way to read, write, and remove SAUCE records and comment blocks from files and buffer arrays. This library is written in C and follows the C99 standard.


## Table of Contents
- [Installation](#installation)
- [Background](#background)
    - [What is SAUCE?](#what-is-sauce)
    - [What is a Record?](#what-is-a-record)
    - [What is a Comment?](#what-is-a-comment)
    - [What functions can I use?](#what-functions-can-i-use)
    - [The EOF Character](#the-eof-character)
    - [Assumptions To Keep In Mind](#assumptions-to-keep-in-mind)
    - [Limitations](#limitations)
- [Reading](#reading)
- [Writing](#writing)
- [Removing](#removing)
- [Performing Checks](#performing-checks)
- [SAUCE struct](#sauce-struct)
- [Constants](#constants)
- [Helper Functions](#helper-functions)
- [Error Codes](#error-codes)


## Installation
### Configure and Build
You can configure and build this repository using the commands below. Note that Release can be replaced with Debug or any other build types provided by CMake.
```bash
git clone https://github.com/marcomer/SAUCE.git
cd SAUCE
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

### Install
Two files can be installed: (1) SauceTool.h to the `${CMAKE_INSTALL_INCLUDEDIR}` and (2) a static libSauceTool library to the `${CMAKE_INSTALL_LIBDIR}` (see [GNUInstallDirs](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html) for details on installation locations).

You can install by running this command in your build directory:
```bash
cmake --install . --config Release
```

Note, the install command may require the use of `sudo`. If you would like to install to somewhere other than your system's default install directory, you can add the `--prefix <my-install-dir>` switch to the install command. If you'd like, you can also just copy SauceTool.h and SauceTool.c into your project and compile them yourself.


### Uninstall
An uninstall script has been provided and can be run using `make uninstall`. However, this script only works if your build directory contains `install_manifest.txt`, which is generated by the install command. If you would prefer to not use the script, you can simply delete the files refered to in `install_manifest.txt`.



## Background

### What is SAUCE?
SAUCE, the Standard Architecture for Universal Comment Extensions, is a protocol for attaching meta data or comments to files. It is mainly intended for ANSI art files, but it can used for many different file types. 

SAUCE has 4 parts: The original file contents, an End-Of-File or [EOF character](#the-eof-character) (Decimal 26, Hex 1A, Ctrl+Z), an optional comment block and the SAUCE record. A SAUCE record should make up the last 128 bytes of a files.

You can see the [official specification](https://www.acid.org/info/sauce/sauce.htm) by Olivier "Tasmaniac" Reubens / ACiD for more information on interpreting SAUCE records.


### What is a Record?
A SAUCE record is found in the last 128 bytes of a file. A record contains several fields (see [official specification](https://www.acid.org/info/sauce/sauce.htm) and [SAUCE struct](#sauce-struct)). In order to be a record, the ID field (i.e. the first 5 bytes) of the record must be equal to the string `"SAUCE"`, null character not included.


### What is a Comment?
A SAUCE record can be paired with an optional comment block. A comment contains two parts, the ID field and the comment lines, which in this library is called the comment string. The comment must be placed immediately before the record in the file. In order to be a comment, the ID field of the comment block (i.e. the first 5 bytes) of the comment block must be equal to the string `"COMNT"`, null character not included. 

Comment blocks **cannot** be read, written, replaced, or removed unless a corresponding SAUCE record also exists.


### What functions can I use?
This library provides 2 distinct sets of functions for **files** and for **buffers** (i.e. char/byte arrays). Functions that access **files** follow a similiar naming convention to the C standard I/O File library (e.g. `SAUCE_fread()`, `SAUCE_Comment_fwrite()`, etc.). Functions that access **buffers** have similiar names but are missing the `f` character (e.g. `SAUCE_read()`, `SAUCE_Comment_write()`, etc.).

The file functions are the most convenient and are adequate for most cases. However, if frequently reopening files is a concern for you or would be impractical, the buffer functions are your solution.

See the Usage section in the [Table of Contents](#table-of-contents) for info on how to use this library.


### The EOF Character
SAUCE specifies that an EOF character (Decimal 26, Hex 1A, Ctrl+Z) should be placed immediately before the SAUCE data as a way to prevent a text/file viewer from reading the SAUCE data as text.

Although EOF characters are important for text/file viewers, this library specifies that an EOF char is *not* required to **read** or **perform checks** on any SAUCE information.

The **write** functions will attempt to add an EOF character. An EOF character will only be added if the function is *certain* that an EOF does not already exist. 

The **remove** functions may remove an EOF character if one exists immediately before the SAUCE data. If you are only removing a comment and an EOF character exists, the EOF will *not* be removed.

The **remove** functions could also add an EOF, but only if you are removing a comment *and* if no EOF character already exists.


### Assumptions To Keep In Mind
1. Any function in this library will **never** allocate memory for you. It is your responsbility to provide allocated buffers, structs, and strings to any of the functions that require it.
2. Buffer functions require a buffer's length, which is often the parameter `n`. Note that `n` isn't the *actual* size of the allocated array, but the length of the file contents present in the buffer. All buffer functions will treat data from index `0` to `n-1` as the provided file contents. If you are attempting to read, replace, or remove a SAUCE record/comment block, bytes `n-1` to `n-128` must contain the SAUCE record.
3. If you are using the buffer functions, it is your responsibility to make sure your buffer array is large enough to hold your file contents, an EOF character, an optional comment block, and a SAUCE record.
4. Unexpected behavior may occur if your file/buffer contains invalid, misplaced, or otherwise non-standard SAUCE records/comments.


### Limitations

#### Correctness of SAUCE Record and Comment Fields
This library does *not* check the fields of SAUCE records for correctness. The only fields that will be checked are the `ID` field and the `Comments` field. This is also similar for comments: only the comment's `ID` will be checked for correctness.

#### File Access
This library provides a safe, but possibly slow solution to reading the bytes immediately before the end of a binary file stream. Since C does not require systems to meaningfully support SEEK_END for binary file streams (see [fseek() documentation](https://en.cppreference.com/w/c/io/fseek)), this library takes a safe approach by reading the file from beginning to end in chunks and extracting the SAUCE record from the last chunk(s). However, when compiled on Windows or POSIX systems, this library will optimize the reading process by instead calling either Windows or POSIX standard functions in order to only read the SAUCE data in a file instead of the entire file in chunks.

File truncation is also implemented in this library using only C standard functions and by creating temporary files with `tmpfile()`. When compiled on Windows or POSIX systems, no temporary files will be created. Instead, files will be quickly truncated using standard functions from Windows or POSIX.

Currently, files over 2GB are not supported.



## Reading
Functions are provided to find and read SAUCE records and CommentBlocks from files/buffers. 

**NOTE**: It is recommended that you first read the SAUCE record to determine how many comment lines exist *before* you attempt to read the comment. However, if you only care for the full comment, attempting to read 255 lines of the comment is guaranteed to give you all available lines.


### Functions
#### `SAUCE_fread(const char* filepath, SAUCE* sauce)`
- From a file, read a SAUCE record into `sauce`.


#### `SAUCE_Comment_fread(const char* filepath, char* comment, uint8_t nLines)`
- From a file, read at most `nLines` of a SAUCE CommentBlock into `comment`. A null character will be appended onto `comment` as well. If the file does not contain a comment or the actual number of lines is less than `nLines`, then expect 0 lines or all lines to be read, respectively.


#### `SAUCE_read(const char* buffer, uint32_t n, SAUCE* sauce)`
- From the first `n` bytes of a buffer, read a SAUCE record into `sauce`.


#### `SAUCE_Comment_read(const char* buffer, uint32_t n, char* comment, uint8_t nLines)`
- From the first `n` bytes of a buffer, read at most `nLines` of a SAUCE CommentBlock into `comment`. A null character will be appended onto `comment` as well. If the buffer does not contain a comment or the actual number of lines is less than `nLines`, then expect 0 lines or all lines to be read, respectively.


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

#### `SAUCE_Comment_fwrite(const char* filepath, const char* comment, uint8_t lines)`
- Write a SAUCE CommentBlock to a file, replacing a CommentBlock if one already exists.
- `lines` is the number of lines to be written. `comment` must be at least `SAUCE_COMMENT_STRING_LENGTH(lines)` bytes long.
- The "Comments" field of the file's SAUCE record will be updated to `lines`.

#### `SAUCE_write(char* buffer, uint32_t n, const SAUCE* sauce)`
- Write a SAUCE record to a buffer.
- If the last 128 bytes of the buffer (bytes `n-1` to `n-128`) contain a SAUCE record, the buffer's SAUCE record will be replaced. Otherwise, the EOF character and the new SAUCE record will be appended to the buffer at index `n`.
- **Important**: To prevent a buffer overflow error when appending a new record, the buffer's actual size must be at least `n` + 129 bytes (the size of a SAUCE record including an EOF character).

#### `SAUCE_Comment_write(char* buffer, uint32_t n, const char* comment, uint8_t lines)`
- Write a SAUCE CommentBlock to a buffer, replacing a CommentBlock if one already exists.
- `lines` is the number of lines to be written. `comment` must be at least `SAUCE_COMMENT_STRING_LENGTH(lines)` bytes long.
- The "Comments" field of the file's SAUCE record will be updated to `lines`.
- If the last 128 bytes of the buffer (bytes `n-1` to `n-128`) contain a SAUCE record, the CommentBlock will be written. Otherwise, an error will be returned.
- The "Comments" field of the buffer's SAUCE record will be updated to the new number of comment lines.
- **Important**: To prevent a buffer overflow error when writing a new comment, the buffer's actual size must be at least `n` + `SAUCE_COMMENT_BLOCK_SIZE(number of comment lines)`.


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

#### `SAUCE_remove(char* buffer, uint32_t n)`
- Remove a SAUCE Record from the first `n` bytes of a buffer, along with the SAUCE CommentBlock if one exists.
- The EOF character will be removed as well.

#### `SAUCE_Comment_remove(char* buffer, uint32_t n)`
- Remove a SAUCE CommentBlock from the first `n` bytes of a buffer.
- The "Comments" field of the buffer's SAUCE record will be set to 0.


### Return Values
On success, all **file** remove functions will return 0. On error, all **file** remove functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.

On success, all **buffer** remove functions will return the new length of the buffer. On error, all **buffer** remove functions will return a negative error code. You can use `SAUCE_get_error()` to get more info about the error.




## Performing Checks
`SAUCE_check_file()` and `SAUCE_check_buffer()` are provided to check for the existence of SAUCE data. The only fields that will be checked for correctness will be the `ID` fields of the record/comment and the `Comments` field of the record.

`SAUCE_equal()` and `SAUCE_Comment_equal()` are provided to check if two SAUCE structs or two SAUCE CommentBlocks are equal.

### Functions
#### `SAUCE_check_file(const char* filepath)`
- Check if a file contains SAUCE data. 

#### `SAUCE_check_buffer(const char* buffer, uint32_t n)`
- Check if the first `n` bytes of a buffer contain SAUCE data.

#### `SAUCE_equal(const SAUCE* first, const SAUCE* second)`
- Determine if two SAUCE records are equal.
- SAUCE records are equal if each field between the SAUCE records match.

#### `SAUCE_Comment_equal(const char* first_comment, const char* second_comment, uint8_t lines)`
- Determine if two SAUCE comments are equal. Both comments must be at least `SAUCE_COMMENT_STRING_LENGTH(lines)` bytes long. Anything beyond the given number of `lines`, including any terminating null characters after the last line, will be not compared or read.

### Return Values

On success, `SAUCE_check_file()` and `SAUCE_check_buffer()` will return 1 (i.e. true) if the file/buffer contained SAUCE data. On error, meaning that no SAUCE data existed or the checked fields were incorrect, the check functions will return 0 (i.e. false). If 0 is returned, you can call `SAUCE_get_error()` to learn more about why the check failed.

The `SAUCE_equal()` and `SAUCE_Comment_equal()` will return a boolean value: 1 for true, and 0 for false.



## `SAUCE` struct
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
string will be returned if no SAUCE error has occurred yet.

### `SAUCE_clear_error()`
Clear the last error message. Will do nothing if no SAUCE error has occurred yet.

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
- `SAUCE_EEMPTY` - The given file/buffer was empty
- `SAUCE_EOTHER` - An error occurred, please call SAUCE_get_error() for latest error message