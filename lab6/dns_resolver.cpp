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
#include <sstream>

#include <iostream>
#include <string.h>
#include <string>
#include <ctype.h>

#include "dns_header.h"


#include <vector>
using std::string;

void send_dns_request();
void create_dns_packet();
void encode_dns_request();
void decode_dns_response();
void name_to_dns_format(unsigned char* dns_head, char* host);
size_t LevenshteinDistance(const std::string& s1, const std::string& s2);
void read_name(unsigned char *buf, int& index, char * name, int data_len);
void read_ptr_name(unsigned char *buf, int offset, char * name, int& name_index, bool &stop);
void convert_name(char * name);
int is_bit_set(int n, int num);
//TODO answer
void decode_authoritative_record(unsigned char* buf, int& k);
void decode_answer_or_additional_record(unsigned char* buf, int& k);
void read_address(unsigned char *buf, int offset, char * name);

int main(int argc, char const *argv[]) {
    /* Main Thread */
    // char* host = "3www6google3com";

    //pick the closest match

    // std::vector<string> ns_chache = {"com.facebook.profile", "com", "com.facebook", "com.youtube", "com.google"};
    // string test_string = "com.gmail";
    //
    // size_t best_dst = 99999;
    // int best_index = 0;
    // for (int i = 0; i < ns_chache.size(); i++) {
    //     /* code */
    //     size_t distance = LevenshteinDistance(ns_chache[i], test_string);
    //     printf("%i\n",distance);
    //     // printf("%i\n", best_dst );
    //     if(distance < best_dst) {
    //         best_dst = distance;
    //         best_index = i;
    //     }
    // }

    //
    // printf("closest match: %s\n", ns_chache[best_index].c_str() );

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

size_t LevenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t m(s1.size());
    const size_t n(s2.size());

    if (m == 0) return n;

    if (n == 0) return m;

    size_t *costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; k++) costs[k] = k;

    size_t i = 0;

    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end();
         ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;

        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end();
             ++it2, ++j)
        {
            size_t upper = costs[j + 1];

            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    size_t result = costs[n];
    delete[] costs;

    return result;
}

void encode_dns_request() {
    int sock_fd;
    struct sockaddr_in dest;
    _dns_header_t* dns_head = new _dns_header_t();

    //TODO get from arg, or set to default
    string root_server = "198.41.0.4";

    sock_fd = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(root_server.c_str());

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

    // dns_head->name = "\x03www\x06google\x03\com\0";
    dns_head->name = "www.google.com\0";

    dns_head->q_type = htons(1);
    dns_head->q_class = htons(1);

    std::stringstream stream;

    dns_head->serialize(stream);

    printf("\nSending Packet...");

    if( sendto(sock_fd, stream.str().c_str(), sizeof(stream), 0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }
    printf("Done\n");
    delete dns_head;

    unsigned char* buf = new unsigned char;
    //Receive the answer

    /* why? is this needed? */
    int x;
    printf("\nReceiving answer...");
    if(recvfrom (sock_fd, buf, 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&x ) < 0)
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


    /* save all authoritative NS's */
    // printf("%s\n", buf[sizeof(dns_head)]);

    // the response portion starts @ (char*)(&buf[sizeof(_dns_header_t)])
    int k = sizeof(_dns_header_t);

    printf("\nAnswer Records : %d \n" , ntohs(dns_head->ans_count) );
    for(int i=0 ; i < ntohs(dns_head->ans_count) ; i++) {
        decode_answer_or_additional_record(buf, k);
    }

    printf("\nAuthoritative Records : %d \n", ntohs(dns_head->auth_count));
    for (int i = 0; i < ntohs(dns_head->auth_count); i++) {
        decode_authoritative_record(buf, k);
    }

    printf("\nAdditional Records : %d \n" , ntohs(dns_head->add_count) );
    for(int i=0 ; i < ntohs(dns_head->add_count) ; i++) {
        /* save the corresponding IPV4 address of the NS's */
        decode_answer_or_additional_record(buf, k);
    }

    return;
}

void read_name(unsigned char *buf, int& index, char * name, int data_len) {
    int start_index = index;
    bool stop = false;
    for (int i = 0; buf[index] != '\0'; i++) {
        /* check if ptr by checking 1100 0000 are set */
        if( is_bit_set(6, buf[index]) > 0 && is_bit_set(7, buf[index]) > 0) {
            /* move to offset byte */
            index++;
            int offset = (int)buf[index];
            read_ptr_name(buf, offset, name, i, stop);
            /* move to next byte */
            index++;

            if(stop) break;
            /* check if read to data_len */
            if(index == (start_index + data_len)) break;

        /* copy chars */
        } else {
            name[i] = buf[index];
            index++;
        }
    }
}

void read_ptr_name(unsigned char *buf, int offset, char * name, int& name_index, bool &stop) {
    int next_offset  = 0;
    int k = 0;
    for (int i = name_index; buf[offset + k] != '\0'; i++) {
        /* check if ptr by checking 1100 0000 are set */
        if( is_bit_set(6, buf[offset + k]) > 0
            && is_bit_set(7, buf[offset + k]) > 0) {
            next_offset = (int)buf[offset + k + 1];

            if(stop) break;

            /* recursive */
            read_ptr_name(buf, next_offset, name, name_index, stop);
        } else {
            name[i] = buf[offset + k];
            name_index++;
            k++;
        }
    }

    stop = true;
}

void convert_name(char * name) {
    int p = 0;
    int i = 0;
    for (i = 0; i < (int)strlen((const char *)name); i++)
    {
        p = name[i];

        for (int j = 0; j < (int)p; j++)
        {
            name[i] = name[i + 1];
            i       = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0';
}

/* helper function used to check if bits are set */
int is_bit_set(int n, int num) {
    return ((1 << n) & num);
}

void decode_authoritative_record(unsigned char* buf, int& k) {
    /* fill our struct */
    _res_record_t *res_rec = new _res_record_t();
    char name[256];
    memset(name, 0, 256);

    /* if name was found without compression, move passed trailing null */
    if (buf[k] == '\0') k++;

    read_name(buf, k, name, 255);

    res_rec->r_name = name;
    printf("NAME: %s", res_rec->r_name);

    /* move to next byte */
    k++;

    /* copy type */
    res_rec->r_type = *((unsigned short *)&buf[k]);
    printf("   TYPE: %d", res_rec->r_type);

    /* move passed read bytes */
    k += 2;

    /* copy class */
    res_rec->r_class = *((unsigned short *)&buf[k]);
    printf("   CLASS: %d", res_rec->r_class);

    /* move passed read bytes */
    k += 2;

    /* don't care about ttl */
    /* move passed the ttl bytes */
    k += 4;

    res_rec->r_data_len = buf[k];
    printf("   D_LENGTH: %d", res_rec->r_data_len);

    /* move passed read bytes */
    k += 1;
    //printf("%s\n", res_rec);
    /* if type is NS read data */
    memset(name, 0, 255);
    //printf("%s\n", res_rec);
    read_name(buf, k, name, res_rec->r_data_len);
    //printf("%s\n", res_rec);
    /* convert to '.' format */
    convert_name(name);
    res_rec->r_data = name;
    printf("   DATA: %s\n", res_rec->r_data);

    /* TODO store nameserver to db */

    delete res_rec;
    // delete name;
}

void decode_answer_or_additional_record(unsigned char* buf, int& k) {
    /* fill our struct */
    _res_record_t *res_rec = new _res_record_t();
    char name[256];
    memset(name, 0, 256);

    /* if name was found without compression, move passed trailing null */
    if (buf[k] == '\0') k++;

    read_name(buf, k, name, 255);
    convert_name(name);
    res_rec->r_name = name;
    printf("NAME: %s", res_rec->r_name);

    /* move to next byte */
    k++;

    /* copy type */
    res_rec->r_type = *((unsigned short *)&buf[k]);
    printf("   TYPE: %d", res_rec->r_type);

    /* move passed read bytes */
    k += 2;

    /* copy class */
    res_rec->r_class = *((unsigned short *)&buf[k]);
    printf("   CLASS: %d", res_rec->r_class);

    /* move passed read bytes */
    k += 2;

    /* don't care about ttl */
    /* move passed the ttl bytes */
    k += 4;

    res_rec->r_data_len = buf[k];
    printf("   D_LENGTH: %d", res_rec->r_data_len);

    /* move passed read bytes */
    k += 1;

    /* if type is NS read data */
    if(res_rec->r_type == IPV4_T) {
        memset(name, 0, 255);
        read_address(buf, k, name);
        // read_name(buf, k, name, res_rec->r_data_len);
        res_rec->r_data = name;
        printf("   DATA: %s\n", res_rec->r_data);
    }
    else {
        printf("   DATA: ipv6 ignored\n");
    }

    k +=res_rec->r_data_len;
    /* TODO store nameserver to db */

    delete res_rec;
    //delete name;
}

void read_address(unsigned char *buf, int offset, char * name) {
    for (int i = 0; i < 4 ; i++) {
        int val = (int)buf[offset + i];
        // itoa(val, name[i], 10);
        char converted_int[2] = {0};
        sprintf(converted_int, "%d", val);
        strcat(name, converted_int);
        if(i+1 != 4) {
            strcat(name, ".");
        }
    }
}
