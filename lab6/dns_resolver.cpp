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

void serialize_dns_packet(char* buf, _dns_packet_t* dns_packet) {

    char* header = {0};
    header = (char*)(dns_packet->header);
    strcpy(buf, header);

    // int start_of_query = sizeof(_dns_packet_t) + 1;
    // _dns_query_t* query = dns_packet->query;
    // strcpy((char*)(buf[start_of_query]), query->qname);
    // strcpy((char*)buf[start_of_query+strlen(query->qname)], (char*)(query->ques));
}

void encode_dns_request() {
    int sock_fd;
    struct sockaddr_in dest;

    _dns_packet_t* dns_packet = new _dns_packet_t();
    _dns_header_t* dns_head = new _dns_header_t();
    _dns_query_t* dns_query = new _dns_query_t();
    _dns_question_t* dns_question = new _dns_question_t();

    // char* root_server = "192.203.230.10";
    char* root_server = "10.101.1.85";

    sock_fd = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(root_server);

    dns_head->trans_id = (unsigned short)htons(getpid());

    dns_head->rd = 0;
    dns_head->tc = 0;
    dns_head->aa = 0;
    dns_head->opcode = 0;
    dns_head->qr = 0;

    dns_head->r_code = 0;
    dns_head->z = 0;
    dns_head->ra = 0;

    dns_head->q_count = htons(1);
    dns_head->ans_count = 0;
    dns_head->auth_count = 0;
    dns_head->add_count = 0;

    //NOTE might have to htons
    dns_question->qtype = htons(1);
    dns_question->qclass = htons(1);

    dns_query->qname = "3www6google3com\0";
    dns_query->ques = dns_question;

    dns_packet->header = dns_head;
    dns_packet->query = dns_query;

    char* packet = {0};
    serialize_dns_packet(packet, dns_packet);
    // int question_name_start = sizeof(struct _DNS_HEADER) + 1;

    // query_name = (char*)&buf[question_name_start];
    // char* name = "3www6google3com0";
    // strcpy(query_name, (char*)name);
    // int dns_question_start = question_name_start + strlen((char*)query_name);
    // printf("%s\n", query_name);
    // printf("%i\n", dns_head->q_count);
    //
    // dns_question =(struct _DNS_QUESTION*)&buf[dns_question_start];
    //
    // dns_question->qtype = 1;
    // dns_question->qclass = 1;


    // printf("stuff: %i\n", (char*)buf);

    printf("\nSending Packet...");

    if( sendto(sock_fd,packet, sizeof(dns_packet),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }
    printf("Done\n");


    delete dns_packet;
    delete dns_head;
    delete dns_query;
    delete dns_question;

    //Receive the answer
    // int i = sizeof dest;
    // printf("\nReceiving answer...");
    // if(recvfrom (sock_fd,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
    // {
    //     perror("recvfrom failed");
    // }
    // printf("Done\n");
    //
    // dns_head = (struct _DNS_HEADER*) buf;

    //move ahead of the dns header and the query field
    // unsigned char* reader = &buf[sizeof(struct _DNS_HEADER) + (strlen((const char*)query_name)+1) + sizeof(struct _DNS_QUESTION)];

    // printf("\nThe response contains : ");
    // printf("\n %d RESPONSE.", dns_head->qr);
    // printf("\n %d r_code.", dns_head->r_code);
    // printf("\n %d Questions.",ntohs(dns_head->q_count));
    // printf("\n %d Answers.",ntohs(dns_head->ans_count));
    // printf("\n %d Authoritative Servers.",ntohs(dns_head->auth_count));
    // printf("\n %d Additional records.\n\n",ntohs(dns_head->add_count));
}
