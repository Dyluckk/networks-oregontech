#pragma once
//*****************************************************
// Declare a function that reads from a file descriptor a line at a time
//
// Author: Phil Howard

#include <sys/types.h>

//**********************************************************
// read from a file descriptor a line at a time
//
// Thread safety: MT safe assuming no two threads access the same fd
//
// buff: the location where the line is to be stored
// size: the size of the buffer
// fd: the file descriptor to read the data from
//
// Note: This function has behavior analagous the fgets.
char *readline(char *buff, ssize_t size, int fd);
