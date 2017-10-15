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

static int port = 50000;

/* test used to ensure testing framework is working properly */
void sanity_test() {
    int a = 1;
    TEST_ASSERT( a == 1 ); //this one will pass
}

void client_test1() {
    const int SERVICE_PORT = 50000;
    struct sockaddr_in myaddr, remaddr;
    int fd, slen=sizeof(remaddr);
    int recvlen;		/* # bytes in acknowledgement message */
    char* server = "127.0.0.1";	/* change this to use a different server */

    /* create a socket */
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        printf("socket created\n");

    /* bind it to all local addresses and pick any port number */
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(0);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
    }

    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server, &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    /* create request */
    request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    strncpy(client_buff->service_name, "www.woo.com", MAX_SERVICE_NAME_LEN);
    client_buff->msg_type = DEFINE_PORT;
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
             recieved_buff = decode(recieved_buff, server_reponse);
             printf("received packet msg_type: \"%d\"\n", recieved_buff->msg_type);
             printf("received packet status: \"%d\"\n", recieved_buff->status);
             printf("received packet service_name: \"%s\"\n", recieved_buff->service_name);
             printf("received packet port: \"%d\"\n", recieved_buff->port);
             free(server_reponse);
             free(recieved_buff);
        }

    close(fd);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(sanity_test);
    RUN_TEST(client_test1);
    return UNITY_END();
}

/*TODO ask phil if we need to validate the args */
//  bool error = false;
//  /* check if valid args */
//  if(!isdigit(service_port)) error = true;
//  cout << error << endl;
//  if(!isdigit(min_supported_ports)) error = true;
//  cout << error << endl;
//  if(!isdigit(keep_alive_time)) error = true;
//  cout << error << endl;
//
//  cout << "service_port: " << service_port << endl;
//  cout << "min_supported_ports: " << min_supported_ports << endl;
//  cout << "keep_alive_time: " << keep_alive_time << endl;
//
//  /* if invalid arg is passed, print error and terminate */
//  if(error) cerr << "invalid arg passed, terminating" << endl;



// void clinet_test1() {
//     const int SERVICE_PORT = port; /* hard-coded port number TODO pass as commmand line arg via -p */
//     const int BUFLEN = 2048;
//
//     struct sockaddr_in myaddr, remaddr;
//     int fd, slen=sizeof(remaddr);
//     string server = "127.0.0.1"; /* change this to use a different server */
//     char buf[BUFLEN];
//
//     /* create a socket */
//     if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1) printf("socket created\n");
//
//     /* bind it to all local addresses and pick any port number */
//     memset((char *)&myaddr, 0, sizeof(myaddr));
//     myaddr.sin_family = AF_INET;
//     myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     myaddr.sin_port = htons(0);
//
//     if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
//         perror("bind failed");
//         return;
//     }
//
//     /* set up the address we want to send messages to */
//     memset((char *) &remaddr, 0, sizeof(remaddr));
//     remaddr.sin_family = AF_INET;
//     remaddr.sin_port = htons(SERVICE_PORT);
//     if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
//         fprintf(stderr, "inet_aton() failed\n");
//         exit(1);
//     }
//
//     /* send messages */
//     printf("Sending packet to port %d\n", SERVICE_PORT);
//     sprintf(buf, "This is a packet");
//
    // /* create request */
    // request_t* client_buff = (request_t*)malloc(sizeof(request_t));
    // strncpy(client_buff->service_name, "woo.com", MAX_SERVICE_NAME_LEN);
    // client_buff->msg_type = DEFINE_PORT;
    // client_buff->status = 0;
    // client_buff->port = 30;
    //
    // /* create & encode request_t to be sent to the ns */
    // void* buffer;
    // buffer = encode(client_buff, client_buff);
    // if (sendto(fd, buffer, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
    //     perror("sendto");
//
//     request_t * server_reponse = (request_t*)malloc(sizeof(request_t));
//     while (true) {
//         /* now receive an acknowledgement from the server */
// 		int recvlen = recvfrom(fd, server_reponse, sizeof(request_t), 0, (struct sockaddr *)&remaddr, &slen);
//         if (recvlen >= 0) {
//             printf("%s\n", "got somethin");
//             free(server_reponse);
//             break;
//         }
//     }
//
//     close(fd);
// }
