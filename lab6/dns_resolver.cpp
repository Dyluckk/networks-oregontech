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
#include <vector>
#include <map>
#include <unordered_map>
#include <utility>
#include <atomic>

#include "dns_header.h"
#include "Locked_Multi.h"
#include "Locked_Map.h"
#include "timed_recvfrom.h"
#include "timed_accept.h"

using std::string;
using std::pair;

typedef std::unordered_map<string, string> temp_ns_cont_t;

string where_to_start(char* name_to_resolve, vector<pair<string,string>> contacted_path);
bool send_dns_request(char* ns_name, char* name_to_resolve, vector<pair<string,string>>& prev_contacted);
string trim_name(char* name_to_resolve);
void name_to_dns_format(unsigned char* dns_head, char* host);
pair<string,string> check_ns_cache_for_label(char* label, vector<pair<string,string>>& prev_contacted);
void read_name(unsigned char *buf, int& index, char * name, int data_len);
void read_ptr_name(unsigned char *buf, int offset, char * name, int& name_index, bool &stop);
void convert_to_dotted_format(char * name);
char* convert_to_dns_format(char* name_to_resolve);
int is_bit_set(int n, int num);
void decode_answer_record(unsigned char* buf, int& k);
void decode_authoritative_record(unsigned char* buf, int& k, temp_ns_cont_t& found_ns);
void decode_additional_record(unsigned char* buf, int& k, temp_ns_cont_t& found_ns);
void read_address(unsigned char *buf, int offset, char * name);

Locked_Multi ns_cache;
Locked_Map resolved_names_cache;

string root_server = "198.41.0.4";

using std::string;
using std::thread;

#define BUFFER 1024
#define MAXPENDING 50
static std::atomic<int> avail_threads;

static std::atomic<bool> g_shutdown;
static std::atomic<bool> g_verbose;

typedef struct
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int server_sock;
    int client_sock;
    int server_port;
    unsigned int client_len;
} server_settings;

int ready_and_bind(server_settings * socket_settings)
{
    if((socket_settings->server_sock =
                socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        return 1;

    memset(&(socket_settings->server_addr), 0,
            sizeof(socket_settings->server_addr));

    socket_settings->server_addr.sin_family = AF_INET;
    socket_settings->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_settings->server_addr.sin_port = htons(socket_settings->server_port);

    int yes = 1;

    if(setsockopt(socket_settings->server_sock, SOL_SOCKET, SO_REUSEADDR,
                &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    //bind
    if(bind(socket_settings->server_sock,
                (struct sockaddr *) &socket_settings->server_addr,
                sizeof(socket_settings->server_addr)) < 0)
        return 2;

    return 0;
}

int start_server(server_settings * srv_info)
{
    int port = 55555;

    srv_info->server_port = port;

    if(ready_and_bind(srv_info) != 0)
        return 1;


    if(listen(srv_info->server_sock, MAXPENDING) < 0)
        return 3;
    printf("Recieving requests on port: %i\n", srv_info->server_port);
    return 0;
}

int parse_args(int argc, char * argv[], char * nameserver)
{
    int c;
    while ((c = getopt (argc, argv, "n:")) != -1)
    {
        switch (c)
        {
          case 'n':
            strcpy(nameserver, optarg);
            break;
          case '?':
            if (optopt == 'n')
              perror("Option -n requires an argument.\n");
            else
              perror("Unknown option character\n");
            return 1;
          default:
            break;
        }
    }
    return 0;
}

int handle_message(int comm_port, string request)
{
    if(request.substr(0, 4) == "dump")
    {
        //TODO dump ns_cache and resolved_names_cache
        printf("dump\n");
    }
    else if(request.substr(0, 7) == "verbose")
    {
        g_verbose = true;
        string response = "output set to verbose";
        if(send(comm_port,response.c_str(), response.length(), 0) !=
                (unsigned)response.length())
            perror("Error sending verbose response");
        close(comm_port);
        printf("verbose\n");
    }
    else if(request.substr(0, 6) == "normal")
    {
        string response = "output set to normal";
        if(send(comm_port,response.c_str(), response.length(), 0) !=
                (unsigned)response.length())
            perror("Error sending normal response");
        close(comm_port);
        g_verbose = false;
        printf("normal\n");
    }
    else if(request.substr(0, 8) == "shutdown")
    {
        string response = "shutting down...";
        printf("Sending %s\n", response.c_str());
        if(send(comm_port, response.c_str(), response.length(), 0) !=
                (unsigned)response.length())
            perror("Error sending shutdown response");
        close(comm_port);
        g_shutdown = true;
        printf("shutdown\n");
    }
    //handle a name lookup
    else
    {
        /* start lookup */

        string response = "";
        vector<pair<string,string>> contacted_path;

        char name_to_resolve[255] = {0};
        strcpy(name_to_resolve, request.c_str());

        string start_address = where_to_start(name_to_resolve, contacted_path);

        bool in_resolved_cache = false;
        if(resolved_names_cache.Find(name_to_resolve) != "") {
            in_resolved_cache = true;
        }

        if(!in_resolved_cache) {
            bool success = send_dns_request((char*)start_address.c_str(), name_to_resolve, contacted_path);
            if(success) {

                printf("ADDRESS FOR %s FOUND@ %s\n",name_to_resolve, resolved_names_cache.Find(name_to_resolve).c_str());

                /* generate and path if verbose is set */
                // if(g_verbose) //TODO generate path

                /* send {name:address} back to client */
                response += "{" + string(name_to_resolve) + ":" + resolved_names_cache.Find(name_to_resolve) + "}";

                /* reply to client */
                if(send(comm_port, response.c_str(), response.length(), 0) !=
                        (unsigned)response.length())
                    perror("error sending response");


            } else {


                printf("ADDRESS NOT FOUND\n");
                /* generate and path if verbose is set */

                /* send {name:NOT FOUND} back to client */
                response += "{" + string(name_to_resolve) + ":" "NOT FOUND" + "}";
                /* reply to client */
                if(send(comm_port, response.c_str(), response.length(), 0) !=
                        (unsigned)response.length())
                    perror("error sending response");

            }
        }
        else {

            /* if verbose is set send
            [retrieved from resolved_cache] {name:address} */

            printf("[retrieved from resolved_cache] ADDRESS FOR %s FOUND@ %s\n",name_to_resolve, resolved_names_cache.Find(name_to_resolve).c_str());

            response += "[retrieved from resolved_cache] {"
                        + string(name_to_resolve)
                        + ":"
                        + resolved_names_cache.Find(name_to_resolve)
                        + "}";

            /* reply to client */
            if(send(comm_port, response.c_str(), response.length(), 0) !=
                    (unsigned)response.length())
                perror("error sending response");

        }

        close(comm_port);
    }

    return 0;
}

void handle_request(int sock)
{
    int comm_port = sock;
    int msg_size = -1;
    char msg_buff[BUFFER] = {0};
    string request = "";
    printf("comm_port: %i\n", comm_port);
    do
    {
        memset(msg_buff, 0, BUFFER);
        if((msg_size = recv(comm_port, msg_buff, BUFFER, 0)) < 0)
        {
            perror("mesage size is < 0");
            break;
        }

        printf("message: %s\n", msg_buff);
        request += msg_buff;
        break;
    }while(msg_buff <= 0);

    request = msg_buff;
    handle_message(comm_port, request);
    ++avail_threads;
}

void wait_for_client(server_settings * srv_info)
{
    printf("Waiting for client\n");
    printf("Port: %i", srv_info->server_sock);
    unsigned int client_sock = 0;
    while(!g_shutdown)
    {
        if(avail_threads > 0)
        {
            if((client_sock = timed_accept(srv_info->server_sock,
                            (struct sockaddr *) &srv_info->client_addr,
                            &srv_info->client_len, 45)) < 0)
                perror("accept timed out");
            else
            {
                std::thread(handle_request, client_sock).detach();
                --avail_threads;
            }
        }
    }

    close(srv_info->server_sock);
    delete srv_info;
}

int main(int argc, char * argv[])
{
    g_shutdown = false;
    g_verbose = false;
    char nameserver[BUFFER];
    avail_threads = 100;
    if(parse_args(argc, argv, nameserver) != 0)
        return 1;
    server_settings * srv_info = new server_settings;
    memset(srv_info, 0, sizeof(server_settings));
    if(start_server(srv_info) != 0)
    {
        delete srv_info;
        return 2;
    }

    wait_for_client(srv_info);

    printf("Closing and shutting down\n");

    return 0;
}

string where_to_start(char* name_to_resolve, vector<pair<string,string>> contacted_path) {
    char label[255] = {0};
    strcpy(label, convert_to_dns_format(name_to_resolve));
    for(;;) {
        pair<string,string> forward_ns = check_ns_cache_for_label(label, contacted_path);

        if(forward_ns.first == "") {
            convert_to_dotted_format(label);
            bool trimmable = false;
            for (int i = 0; label[i] != '\0'; i++) {
                if(label[i] == '.') trimmable = true;
            }
            if(trimmable) {
                strcpy(label, trim_name(label).c_str());
                strcpy(label, convert_to_dns_format(label));
            }
            else {
                return root_server;
            }
        }
        else {
            return forward_ns.second;
        }

    }

}

bool send_dns_request(char* ns_name, char* name_to_resolve, vector<pair<string,string>>& prev_contacted) {
    int sock_fd;
    struct sockaddr_in dest;
    _dns_header_t* dns_head = new _dns_header_t();

    //TODO get from arg, or set to default
    string root_server = ns_name;

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

    dns_head->name = name_to_resolve;

    dns_head->q_type = htons(1);
    dns_head->q_class = htons(1);

    std::stringstream stream;

    dns_head->serialize(stream);

    printf("CONTACTING %s QUERY %s\n", ns_name,name_to_resolve);

    printf("\nSending Packet...");

    if( sendto(sock_fd, stream.str().c_str(), sizeof(stream), 0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }
    printf("Done\n");
    delete dns_head;

    unsigned char buf[65536];
    //Receive the answer

    int x;
    printf("\nReceiving answer...");
    if(timed_recvfrom(sock_fd, buf, 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&x, 3) < 0)
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

    // the response portion starts @ (char*)(&buf[sizeof(_dns_header_t)])
    // int k = (sizeof(unsigned short) * 8) + sizeof()
    int k = strlen(name_to_resolve) + (sizeof(unsigned short) * 8) + 2;

    /* temp storage for names to be put in cache */
    temp_ns_cont_t found_ns;

    printf("\nAnswer Records : %d \n" , ntohs(dns_head->ans_count) );
    for(int i=0 ; i < ntohs(dns_head->ans_count) ; i++) {
        decode_answer_record(buf, k);
    }

    if( ntohs(dns_head->auth_count) >= 1 &&  ntohs(dns_head->add_count >= 1)) {
        printf("\nAuthoritative Records : %d \n", ntohs(dns_head->auth_count));
        for (int i = 0; i < ntohs(dns_head->auth_count); i++) {
            decode_authoritative_record(buf, k, found_ns);
        }

        printf("\nAdditional Records : %d \n" , ntohs(dns_head->add_count) );
        for(int i=0 ; i < ntohs(dns_head->add_count) ; i++) {
            /* save the corresponding IPV4 address of the NS's */
            decode_additional_record(buf, k, found_ns);
        }

        /* check if name was put into resolved_names_cache */
        if(resolved_names_cache.Find(name_to_resolve) != "") return true;

        /* if not resolved look for name_to_resolve in ns_chache */
        char label[255] = {0};
        strcpy(label, convert_to_dns_format(name_to_resolve));
        for(;;) {
            pair<string,string> forward_ns = check_ns_cache_for_label(label, prev_contacted);
            if(forward_ns.first == "") {
                convert_to_dotted_format(label);
                bool trimmable = false;
                for (int i = 0; label[i] != '\0'; i++) {
                    if(label[i] == '.') trimmable = true;
                }
                if(trimmable) {
                    strcpy(label, trim_name(label).c_str());
                    strcpy(label, convert_to_dns_format(label));
                }
                else {
                    return false;
                }
            }
            else {
                bool answered = send_dns_request((char*)forward_ns.second.c_str(), name_to_resolve, prev_contacted);
                if(answered) return true;
            }
        }
    }

    return false;
}

pair<string,string> check_ns_cache_for_label(char* label, vector<pair<string,string>>& prev_contacted) {
    MIterPair it = ns_cache.GetNS(label);
    while(it.first != it.second)
    {
        // printf("key: ");
        // printf("%s  ", it.first->first.c_str());
        // printf("ns: ");
        // printf("%s  ", it.first->second.first.c_str());
        // printf("addr: ");
        // printf("%s\n", it.first->second.second.c_str());

        pair<string, string> lookup;
        lookup.first = it.first->second.first;
        lookup.second = it.first->second.second;

        // char lookup[it.first->second.second.size()+1];
        // strcpy(lookup, it.first->second.second.c_str());

        bool found = false;

        for(vector<pair<string,string>>::const_iterator it = prev_contacted.begin(); it != prev_contacted.end(); it++) {
                // process i
                if(it->first == lookup.first) found = true;
                // cout << it->first << "\n"; // this will print all the contents of *features*
        }

        /* return ns to contact */
        if(!found) {
            pair<string, string> contacted;
            contacted.first = lookup.first;
            contacted.second = lookup.second;
            prev_contacted.push_back(contacted);
            return lookup;
        }

        it.first++;
    }

    /* exhausted ns_cache, name */
    pair<string,string> empty;
    empty.first = "";
    empty.second = "";
    return empty;
}

string trim_name(char* name_to_resolve) {
    int trimmed_sub_str_len = 0;
    char trimmed_name[255] = {0};

    for (int i = 0; name_to_resolve[i] != '.'; i++) {
        if(name_to_resolve[i + 1] == '.') trimmed_sub_str_len = i + 1;
    }

    for (int i = trimmed_sub_str_len + 1, k = 0; name_to_resolve[i] != '\0'; i++, k++) {
        trimmed_name[k] = name_to_resolve[i];
    }

    string trimmed(trimmed_name);
    return trimmed;
}

char* convert_to_dns_format(char* name_to_resolve) {
    string converted_name = "";
    unsigned char before_dot_count = 0;
    int prev_index = 0;
    int index_counter = 0;
    bool scanning = true;
    bool done = false;

    for(int i = 0; !done; i++) {

        if(scanning) {
            if(name_to_resolve[i] == '.' || name_to_resolve[i]== '\0') {
                scanning = false;
                converted_name.push_back(before_dot_count);
                before_dot_count = '\x00';
                i = prev_index - 1;
                index_counter++;
            }
            else {
                before_dot_count++;
                index_counter++;
            }
        }
        else {
            /* copy char to converted_name*/
            if(name_to_resolve[i] != '.' && name_to_resolve[i] != '\0') {
                converted_name.push_back(name_to_resolve[i]);
            }
            /* stop copying if null reached */
            else if(name_to_resolve[i] == '\0') {
                done = true;
            }
            else {
                prev_index += index_counter;
                index_counter = 0;
                scanning = true;
            }
        }
    }
    converted_name.push_back('\x00');

    return (char*)converted_name.c_str();
}

void decode_authoritative_record(unsigned char* buf, int& k, temp_ns_cont_t& found_ns) {

    /* fill our struct */
    _res_record_t *res_rec = new _res_record_t();
    char name[256];
    memset(name, 0, 256);

    /* if name was found without compression, move passed trailing null */
    if (buf[k] == '\0') k++;

    read_name(buf, k, name, 255);

    size_t len = strlen(name)+1;
    res_rec->r_name = new char [len]; // allocate for string and ending \0
    strcpy(res_rec->r_name, name);

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

    memset(name, 0, 255);
    read_name(buf, k, name, res_rec->r_data_len);

    /* convert to '.' format */
    convert_to_dotted_format(name);
    len = strlen(name)+1;
    res_rec->r_data = new char [len]; // allocate for string and ending \0
    strcpy(res_rec->r_data, name);
    printf("   DATA: %s\n", res_rec->r_data);

    found_ns.insert({res_rec->r_data, res_rec->r_name});

    delete res_rec->r_name;
    delete res_rec->r_data;
    delete res_rec;
}

void decode_additional_record(unsigned char* buf, int& k, temp_ns_cont_t& found_ns) {
    /* fill our struct */
    _res_record_t *res_rec = new _res_record_t();
    char name[256];
    memset(name, 0, 256);

    /* if name was found without compression, move passed trailing null */
    if (buf[k] == '\0') k++;

    read_name(buf, k, name, 255);
    convert_to_dotted_format(name);

    size_t len = strlen(name)+1;
    res_rec->r_name = new char [len]; // allocate for string and ending \0
    strcpy(res_rec->r_name, name);

    // res_rec->r_name = name;
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
        len = strlen(name)+1;
        res_rec->r_data = new char [len]; // allocate for string and ending \0
        strcpy(res_rec->r_data, name);

        printf("   DATA: %s\n", res_rec->r_data);


        // Declare an iterator to unordered_map
        temp_ns_cont_t::iterator it;

        it = found_ns.find(res_rec->r_name);

        // Check if iterator points to end of map
        if (it != found_ns.end()) {
            // std::cout << "Element Found - ";

            std::pair<string, string> ns_and_addr;
            string key = it->second;
            ns_and_addr.first = it->first;
            ns_and_addr.second = res_rec->r_data;

            ns_cache.Emplace(key, ns_and_addr);

            // std::cout << key << " " << it->first  << " " << res_rec->r_data << "\n\n\n";
        }

        delete res_rec->r_data;

    }
    else {
        printf("   DATA: ipv6 ignored\n");
    }

    k +=res_rec->r_data_len;
    /* TODO store nameserver to db */

    delete res_rec->r_name;
    delete res_rec;
}

void decode_answer_record(unsigned char* buf, int& k) {
    /* fill our struct */
    _res_record_t *res_rec = new _res_record_t();
    char name[256];
    memset(name, 0, 256);

    /* if name was found without compression, move passed trailing null */
    if (buf[k] == '\0') k++;

    read_name(buf, k, name, 255);
    convert_to_dotted_format(name);

    size_t len = strlen(name)+1;
    res_rec->r_name = new char [len]; // allocate for string and ending \0
    strcpy(res_rec->r_name, name);

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
        len = strlen(name)+1;
        res_rec->r_data = new char [len]; // allocate for string and ending \0
        strcpy(res_rec->r_data, name);
        // read_name(buf, k, name, res_rec->r_data_len);
        printf("   DATA: %s\n", res_rec->r_data);

        /* save to resolved names */
        resolved_names_cache.Add(res_rec->r_name, res_rec->r_data);

    }
    else {
        printf("   DATA: ipv6 ignored\n");
    }

    k +=res_rec->r_data_len;
    /* TODO store nameserver to db */

    delete res_rec;
    //delete name;
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

void convert_to_dotted_format(char * name) {
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
