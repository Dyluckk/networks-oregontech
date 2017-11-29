#include "timed_recvfrom.h"

int timed_recvfrom(int sockfd, void *buf, size_t len, int flags,
               struct sockaddr *src_addr, socklen_t *addrlen, int timeout)
// int timed_recvfrom(int sockfd,
//                    struct sockaddr *cli_addr,
//                    socklen_t *clilen,
//                    int timeout)
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
        return recvfrom (sockfd, buf, len , flags, src_addr, addrlen);
    }
    else
    {
        return result;
    }
}
