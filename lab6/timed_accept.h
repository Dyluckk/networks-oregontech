#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

int timed_accept(int sockfd, struct sockaddr *cli_addr, socklen_t *clilen,
        int timeout);
