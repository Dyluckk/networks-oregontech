#pragma once
/* * * * * * * * * * * * * * * * * * * * * * * * *
 *  @file    timedread.h
 *  @author  Zachary Wentworth
 *  @email   zachary.wentworth@oit.edy
 *  @date    11/07/2017
 * * * * * * * * * * * * * * * * * * * * * * * * */
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   function modifies the read() method timeout after a designated amount of
*   seconds
*
*   params:
*       + int sockfd - the socket descritor to use for the read
*       + char* buffer - the buffer to read
*       + size_t buff_size - the size of the buffer being returned
*       + int timeout - the wait time until timeout is seconds
*
*   return:
*       ON SUCCESS - result of read()
*       ON FAIL - <=0
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int timed_read(int sockfd, char* buffer, size_t buff_size, int timeout);
