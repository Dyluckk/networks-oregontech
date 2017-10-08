//**************************************************
// Declarations for utilities for converting addresses to strings
// and for extracting port numbers
//
// Author: Philip Howard

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "addr2str.h"

#ifdef __cplusplus
extern "C" {
#endif

char *addr2str(struct sockaddr_in *addr, char *buff)
{
    static char s_buff[100];

    if (buff==NULL) buff = s_buff;
    sprintf(buff, "%s:%d", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    return buff;
}


// Convert a socket file descriptor to a string
// buff must be a pointer to a buffer large enough to hold the text addr.
// If buff==NULL, a static buffer will be used. NOT THREAD SAFE.
char *sock2str(int fd, char *buff)
{
    static char s_buff[100];
    struct sockaddr_in addr;
    int result;
    socklen_t addr_len = sizeof(addr);

    addr.sin_family = AF_INET;
    result = getsockname (fd, (struct sockaddr *)&addr, &addr_len);

    if (buff==NULL) buff = s_buff;

    if (result == 0)
        sprintf(buff, "%d:%s:%d",
                fd, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    else
        strcpy(buff, "Invalid FD");

    return buff;
}

// Determine the port number assigned to a network address
int addr2port(struct sockaddr_in *addr)
{
    return ntohs(addr->sin_port);
}

// Determine the port number assigned to a network file descriptor
int sock2port(int socket)
{
    struct sockaddr_in addr;
    int result;
    socklen_t addr_len = sizeof(addr);

    addr.sin_family = AF_INET;
    result = getsockname (socket, (struct sockaddr *)&addr, &addr_len);

    if (result == 0)
        return ntohs(addr.sin_port);
    else
        return -1;
}

#ifdef __cplusplus
}
#endif
