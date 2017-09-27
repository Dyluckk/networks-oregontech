#pragma once
//******************************************************
// Declare functions for opening and reading files a buffer at a time
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
//******************************************************

#include <sys/types.h>

// Number of characters read at a time
#define BLOCK_SIZE 32

// ***************************************
// Open a file for reading a block at a time
//
// Thread safety: MT safe
//
// name: null terminated string containing the path for the file to be opened
//
// Return value: 
//      On success, returns the file descriptor (a non-negative value) for the
//      file that was opened.
//
//      On failure, returns a negative value, and errno is set to indicate
//      what the failure was
int open_blocks(const char *name);

//****************************************
// Close a file opened by open_blocks
//
// Thread safety: MT safe
//
// fd: The file descriptor to be closed
//
// Return value:
//      on success: returns zero
//      on failure: returns -1 and errno is set appropriately.
//
// Note: This function is just a wrapper for:
//      close(fd)
int close_blocks(int fd);

//*****************************************
// Reads up to BLOCK_SIZE bytes from the file descriptor
//
// Thread safety: MT safe
//
// fd: file descriptor to read from
// buff: the buffer to place the characters into. buff must point to a region
//       that is at least BLOCK_SIZE bytes in size.
//
// Return value:
//      on success: returns the number of bytes read. zero indicates end of file
//      on failure: returns -1 and errno is set appropriately.
//
// Note: this function is just a wrapper for:
//      read(fd, buff, BLOCK_SIZE);
ssize_t read_block(int fd, void *buff);


