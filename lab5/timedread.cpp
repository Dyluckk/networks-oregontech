/* * * * * * * * * * * * * * * * * * * * * * * * *
 *  @file    timedread.cpp
 *  @author  Zachary Wentworth
 *  @email   zachary.wentworth@oit.edy
 *  @date    11/07/2017
 * * * * * * * * * * * * * * * * * * * * * * * * */
#include "timedread.h"

/* Docs in header * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int timed_read(int sockfd, char* buffer, size_t buff_size,
        int timeout)
{
    int result;
    struct timeval tv;
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    tv.tv_sec = (long)timeout;
    tv.tv_usec = 0;

    result = select(sockfd+1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);
    if(result > 0)
    {
        return read(sockfd, buffer, buff_size);
    }
    else
    {
        return result;
    }
}
