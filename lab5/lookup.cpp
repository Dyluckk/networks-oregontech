/* * * * * * * * * * * * * * * * * * * * * * * * *
 *  @file    lookup.cpp
 *  @author  Zachary Wentworth
 *  @email   zachary.wentworth@oit.edu
 *  @date    11/07/2017
 *
 *  @description:
 *  program takes an arguement of a host i.e.
 *  "www.google.com" and it will return a list of
 *  available addresses and their type and whether
 *  or not it is a reliable connection.
 * * * * * * * * * * * * * * * * * * * * * * * * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>

#include "timedread.h"

using std::string;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   function used to attempt connections and communication
*
*   params:
*       + int sockfd - the socket descritor to use for connect and read
*       + addrinfo* rp - the addrinfo of the desired host
*       + char* host - the string of the ip
*
*   return:
*       none
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void test_connection_and_communication(int sockfd, addrinfo* rp, char* host) {
    const char* httpget = "GET / HTTP/1.1\r\n\r\n";
    char buffer[1024] = {0};

    printf("connection to %s ", host);
    if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
        printf("Succeeded. ");

        write(sockfd, httpget, strlen(httpget));
        int nbytes = timed_read(sockfd, buffer, 1024, 8);
        if(nbytes <= 0) {
            printf("Failed.\n");
        }
        else {
            printf("Succeeded.\n");
        }

    } else {
        printf("Failed.\n");
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   main of the application
*
*   params:
*       + argv[1] - host name i.e. "www.google.com"
*
*   return:
*       ON SUCCESS - 0
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(int argc, const char **argv) {

    struct addrinfo* rp;      //used for traversing the getaddrinfo()
    char host[256];           //the address of the host
    string address;           //the adress from the cmd line arg
    struct addrinfo* infoptr; //pointer for linked list of addrinfo's

    /* check for correct number of arguments */
    if ((argc < 2) || (argc > 3)) {
        fprintf(stderr, "Usage: %s  [<address>]\n", argv[0]);
        exit(1);
    }

    /* get host name from command line args */
    address = string(argv[1]);
    /* get address info */
    int result = getaddrinfo(address.c_str(), "80", NULL, &infoptr);
    if (result) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        exit(1);
    }

    /* print lookup table header */
    printf("TYPE              IP            Connection & Communication \n");
    printf("---------------------------------------------------------- \n");

    /* while (!at end of list) */
    for (rp = infoptr; rp != NULL; rp = rp->ai_next) {
        /* get host address */
        getnameinfo(rp->ai_addr, rp->ai_addrlen, host, sizeof(host),
                    NULL, 0, NI_NUMERICHOST);

        /* print socket type */
        if (rp->ai_socktype == SOCK_STREAM) printf("TCP ");
        else if (rp->ai_socktype == SOCK_DGRAM) printf("UDP ");
        else if (rp->ai_socktype == SOCK_RAW) printf("RAW ");
        else printf("OTHER ");

        /* create socket */
        int sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd < 0) {
            printf("connection to %s Failed. unable to create socket\n", host);
        } else {
            /* attempt TCP connection */
            if(rp->ai_socktype == SOCK_STREAM) {
                test_connection_and_communication(sockfd, rp, host);
            }
            /* attempt UDP connection */
            else if(rp->ai_socktype == SOCK_DGRAM) {
                test_connection_and_communication(sockfd, rp, host);
            }

            /* close connection */
            close(sockfd);
        }
    }
    /* free address info */
    freeaddrinfo(infoptr);

    return 0;
}
