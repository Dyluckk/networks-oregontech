//******************************************************
// Declare functions for opening and reading files a buffer at a time
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
//******************************************************
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "readblocks.h"

// *************************************
// See the header file for documentation
int open_blocks(const char *name)
{
    int fd;

    fd = open(name, O_RDONLY);

    // limit the number of open file descriptors to 1024
    if (fd >= 1024)
    {
        close(fd);
        return -1;
    }

    return fd;
}

// *************************************
// See the header file for documentation
int close_blocks(int fd)
{
    return close(fd);
}

// *************************************
// See the header file for documentation
ssize_t read_block(int fd, void *buff)
{
    return read(fd, buff, BLOCK_SIZE);
}

