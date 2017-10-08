#pragma once
//**************************************************
// Declarations for utilities for converting addresses to strings
// and for extracting port numbers
//
// Author: Philip Howard

#include <sys/socket.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

// Convert a socket address to a string
// buff must be a pointer to a buffer large enough to hold the text addr.
// If buff==NULL, a static buffer will be used. NOT THREAD SAFE.
char *addr2str(struct sockaddr_in *addr, char *buff);

// Convert a socket file descriptor to a string
// buff must be a pointer to a buffer large enough to hold the text addr.
// If buff==NULL, a static buffer will be used. NOT THREAD SAFE.
char *sock2str(int socket, char *buff);

// Determine the port number assigned to a network address
int addr2port(struct sockaddr_in *addr);

// Determine the port number assigned to a network file descriptor
int sock2port(int socket);

#ifdef __cplusplus
}
#endif
