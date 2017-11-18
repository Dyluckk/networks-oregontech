#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#include <string.h>
#include <string>

#include "dns_header.h"

using std::string;

void send_dns_request();
void create_dns_packet();
void encode_dns_request();
void decode_dns_response();
void name_to_dns_format(unsigned char* dns_head, char* host);

int main(int argc, char const *argv[]) {
    /* Main Thread */
    // char* host = "3www6google3com";
    encode_dns_request();
    /* setup TCP connections for clients */

    /* MULTI_THREAD on accept (use timed accept probably like in prev labs) */

        /* get echo message (check if in correct format) */

        /* parse message */

        /* handle message for the following */
            //dump
            //verbose
            //normal
            //shutdown
            //name - this is just a text string to look up via dns_head
            //invalid request (respond with error)

        /* setup dns_head based on args */

        /* encode dns_head request */

        /* send request */

        /* recv response (timeout) */

            /* decode response */

    return 0;
}

//TODO fix
void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");

    for(i = 0 ; i < strlen((char*)host) ; i++)
    {
        if(host[i]=='.')
        {
            *dns++ = i-lock;
            for(;lock<i;lock++)
            {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}

void encode_dns_request() {
    int sock_fd;
    struct sockaddr_in dest;
    unsigned char buf[512] = {0};
    struct _dns_header_t* dns_head = NULL;
    struct QUESTION *qinfo = NULL;
    unsigned char *qname;

    char* root_server = "198.41.0.4";

    sock_fd = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(root_server);

    dns_head = (struct _dns_header_t*)&buf;

    dns_head->trans_id = (unsigned short)htons(getpid());

    dns_head->rd = 0;
    dns_head->tc = 0;
    dns_head->aa = 0;
    dns_head->opcode = 0;
    dns_head->qr = 0;

    dns_head->r_code = 0;
    dns_head->cd = 0;
    dns_head->ad = 0;
    dns_head->z = 0;
    dns_head->ra = 0;

    dns_head->q_count = htons(1);
    dns_head->ans_count = 0;
    dns_head->auth_count = 0;
    dns_head->add_count = 0;

    unsigned char host[100];

    //Get the hostname from the terminal
    printf("Enter Hostname to Lookup : ");
    scanf("%s" , host);

    //point to the query portion
    qname =(unsigned char*)&buf[sizeof(struct _dns_header_t)];

    ChangetoDnsNameFormat(qname , host);
    qinfo =(struct QUESTION*)&buf[sizeof(struct _dns_header_t) + (strlen((const char*)qname) + 1)]; //fill it

    qinfo->qtype = htons(1); //type of the query , A , MX , CNAME , NS etc
    qinfo->qclass = htons(1); //its internet (lol)

    printf("\nSending Packet...");

    if( sendto(sock_fd,(char*)buf, 256, 0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }
    printf("Done\n");

    //Receive the answer
    int i;
    printf("\nReceiving answer...");
    if(recvfrom (sock_fd,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
    {
        perror("recvfrom failed");
    }
    printf("Done\n");

    dns_head = (_dns_header_t*) buf;

    printf("\nThe response contains : ");
    printf("\n %d RESPONSE.", dns_head->qr);
    printf("\n %d r_code.", dns_head->r_code);
    printf("\n %d Questions.",ntohs(dns_head->q_count));
    printf("\n %d Answers.",ntohs(dns_head->ans_count));
    printf("\n %d Authoritative Servers.",ntohs(dns_head->auth_count));
    printf("\n %d Additional records.\n\n",ntohs(dns_head->add_count));
}
