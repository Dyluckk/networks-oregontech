#include "./lib/unity/unity.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "./src/nameserver.h"
#include "./src/encode.h"
#include "./src/encode.c"

using std::string;
using std::cout;
using std::endl;
using std::cerr;

static int service_port = 50000;

void print_results(request_t * recieved_buff) {
    printf("received packet msg_type: \"%d\"\n", recieved_buff->msg_type);
    printf("received packet status: \"%d\"\n", recieved_buff->status);
    printf("received packet service_name: \"%s\"\n", recieved_buff->service_name);
    printf("received packet port: \"%d\"\n", recieved_buff->port);
}

/* test used to ensure testing framework is working properly */
void sanity_test() {
    int a = 1;
    TEST_ASSERT( a == 1 ); //this one will pass
}

void define_port_valid() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen;		/* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    string test_service_name = "www.woo.com";
    int test_port = 30;
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = DEFINE_PORT;
    client_buff->status = 0;
    client_buff->port = test_port;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == SUCCESS );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);
             TEST_ASSERT( recieved_buff->port == test_port );
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void define_port_service_in_use() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    string test_service_name = "www.woo.com";
    int test_port = 30;
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = DEFINE_PORT;
    client_buff->status = 0;
    client_buff->port = test_port;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == SERVICE_IN_USE );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);
             TEST_ASSERT( recieved_buff->port == test_port );
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void lookup_port_valid() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    string test_service_name = "www.woo.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = LOOKUP_PORT;
    client_buff->status = 0;
    client_buff->port = 0;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             const int EXPECTED_PORT = 30;
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == SUCCESS );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);
             TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void lookup_port_not_found() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    string test_service_name = "www.www.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = LOOKUP_PORT;
    client_buff->status = 0;
    client_buff->port = 0;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             const int EXPECTED_PORT = 0;
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == SERVICE_NOT_FOUND );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);
             TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void keep_alive_valid() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    string test_service_name = "www.woo.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = KEEP_ALIVE;
    client_buff->status = 0;
    client_buff->port = 30;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             const int EXPECTED_PORT = 30;
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == SUCCESS );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);
             TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void keep_alive_invalid_port() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    const int EXPECTED_PORT = 40;
    string test_service_name = "www.woo.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = KEEP_ALIVE;
    client_buff->status = 0;
    client_buff->port = EXPECTED_PORT;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == INVALID_ARG );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);
             TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void remove_invalid_port() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    const int EXPECTED_PORT = 40;
    string test_service_name = "www.woo.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = CLOSE_PORT;
    client_buff->status = SUCCESS;
    client_buff->port = EXPECTED_PORT;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == INVALID_ARG );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);

             TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);

             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void remove_invalid_service_name() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    const int EXPECTED_PORT = 30;
    string test_service_name = "www.www.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = CLOSE_PORT;
    client_buff->status = SUCCESS;
    client_buff->port = EXPECTED_PORT;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE);
             TEST_ASSERT( recieved_buff->status == SERVICE_NOT_FOUND);
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);

             TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);

             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void remove_valid() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    const int EXPECTED_PORT = 30;
    string test_service_name = "www.woo.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = CLOSE_PORT;
    client_buff->status = SUCCESS;
    client_buff->port = EXPECTED_PORT;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE);
             TEST_ASSERT( recieved_buff->status == SUCCESS);
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);

             TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);

             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void empty_request_bad() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");

    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create & encode request_t to be sent to the ns */
    void* buffer;

    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
            //  const int EXPECTED_PORT = 0;
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == INVALID_ARG );
            //  int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
            //  TEST_ASSERT( service_name_cmp_result == 0);
            //  TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void invalid_status_request_bad() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen; /* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");

    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    string test_service_name = "www.www.com";
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = 123;
    client_buff->status = 0;
    client_buff->port = 0;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
            //  const int EXPECTED_PORT = 0;
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == INVALID_ARG );
            //  int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
            //  TEST_ASSERT( service_name_cmp_result == 0);
            //  TEST_ASSERT( recieved_buff->port == EXPECTED_PORT);
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

void stop_name_server() {
    const int SERVICE_PORT = service_port;
    struct sockaddr_in remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen;		/* # bytes in acknowledgement message */
    string server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");


    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    string test_service_name = "www.woo.com";
    int test_port = 30;
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, test_service_name.c_str(), MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = STOP;
    client_buff->status = 0;
    client_buff->port = test_port;

    /* create & encode request_t to be sent to the ns */
    void* buffer;
    buffer = encode(client_buff, client_buff);
    if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
        perror("sendto");

    free(client_buff);

    request_t * recieved_buff = (request_t*)malloc(sizeof(request_t));
    request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
    /* now receive an acknowledgement from the server */
    recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, (socklen_t*)&slen);
        if (recvlen >= 0) {
             recieved_buff = decode(recieved_buff, server_reponse);
             TEST_ASSERT( recieved_buff->msg_type == RESPONSE );
             TEST_ASSERT( recieved_buff->status == SUCCESS );
             int service_name_cmp_result = strcmp(recieved_buff->service_name, test_service_name.c_str());
             TEST_ASSERT( service_name_cmp_result == 0);
             TEST_ASSERT( recieved_buff->port == test_port );
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

int main(int argc, char** argv) {
    int option = 0;
    while ((option = getopt(argc, argv,"p:")) != -1) {
        switch (option) {
             case 'p' : service_port = atoi(optarg);
                 break;
        }
    }

    UNITY_BEGIN();
        RUN_TEST(sanity_test);
        RUN_TEST(define_port_valid);
        RUN_TEST(define_port_service_in_use);
        RUN_TEST(lookup_port_valid);
        RUN_TEST(lookup_port_not_found);
        RUN_TEST(keep_alive_valid);
        RUN_TEST(keep_alive_invalid_port);
        RUN_TEST(empty_request_bad);
        RUN_TEST(remove_invalid_port);
        RUN_TEST(remove_invalid_service_name);
        RUN_TEST(remove_valid);
        // RUN_TEST(invalid_status_request_bad);
        /* MUST BE LAST */
        RUN_TEST(stop_name_server);
    return UNITY_END();
}
