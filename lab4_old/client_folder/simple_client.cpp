#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <fstream>

using std::string;
using std::cin;
using std::cout;
using std::ofstream;

#define RCVBUFSIZE 50   /* Size of receive buffer */
const int BUFFER = 2048;

typedef enum {GET, EXIT, SHUTDOWN} req_option;

typedef struct 
{
    int sock;
    struct sockaddr_in server_addr;
    unsigned short server_port;
    string server_ip;
    string req_string;
    char buffer[RCVBUFSIZE];
    unsigned int string_len;
    int bytes_recv;
    int total_bytes_recv;
    string command_string;
}serv_info;

/* Error handling function */
void DieWithError(string errorMessage)
{
    perror(errorMessage.c_str());
    exit(1);
}

int send_exit(serv_info * info)
{
    return 0;
}

int send_get(serv_info * info)
{
    return 0;
}

int send_shutdown(serv_info * info)
{
    return 0;
}


int startup(serv_info * info)
{

    info->server_ip = "127.0.0.1";


    if ((info->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
        return 1;

    memset(&info->server_addr, 0, sizeof(info->server_addr));     /* Zero out structure */
    info->server_addr.sin_family = AF_INET;             /* Internet address
                                                           family */
    info->server_addr.sin_addr.s_addr = inet_addr(info->server_ip.c_str());   /* Server IP address */
    info->server_addr.sin_port = htons(info->server_port); /* Server port */

    return 0;
}

int copy_file(string filename, serv_info * ftp_info)
{
    ofstream file;
    file.open(filename.c_str(), std::ios::out | std::ios::trunc);
    if(file.fail())
        return 1;

    int bytes_recv = 0;
    char buffer[BUFFER];
    memset(buffer, 0, BUFFER);
    int i = 0;
        
    printf("COPYING FILE\n");
    while(bytes_recv >= 0)
    {
        if ((bytes_recv=recv(ftp_info->sock, buffer, BUFFER, 0)) <= 0) 
            bytes_recv = -1;
        ++i;
        file << buffer;
        memset(buffer, 0, BUFFER);
    }

    file.close();

    return 0;
}
int send_req(serv_info * info)
{
    int connection = 0;
    if ((connection = connect(info->sock, (struct sockaddr *)&info->server_addr,
                sizeof(info->server_addr)) < 0)) DieWithError("connect() failed");

    if (send(info->sock, info->command_string.c_str(), info->command_string.length() , 0) 
            != (unsigned)info->command_string.length()) 
        DieWithError("send() sent a different number of bytes than expected");



    info->total_bytes_recv = 0;
    memset(info->buffer, 0, BUFFER);
    while (info->bytes_recv == 0)
    {
        if ((info->bytes_recv=recv(info->sock, info->buffer, BUFFER, 0)) <= 0) 
            DieWithError("recv() failed or connection closed prematurely");
        printf("bytes recieved: %i\n", info->bytes_recv);
    }
    //switch to other socket
    string ftp_info_str = string(info->buffer);
    printf("Full string: %s\n", info->buffer);

    string cleaned = ftp_info_str.substr(ftp_info_str.find(": ") + 2);
    string filename = cleaned.substr(0, cleaned.find(" "));
    string port = cleaned.substr(cleaned.find(" ") + 1);

    serv_info * ftp_info = new serv_info;
    ftp_info->server_port = std::stoi(port);
    int start_res = startup(ftp_info);
    printf("start result: %i\n", start_res);
    if(start_res != 0)
    {
        close(ftp_info->sock);
        return 2;
    }

    if(connect(ftp_info->sock, (struct sockaddr *)&ftp_info->server_addr, sizeof(ftp_info->server_addr)) < 0)
        DieWithError("Unable to connect to ftp port");


    int file_res = copy_file(filename, ftp_info);
    printf("copy result: %i\n", file_res);
    if(file_res != 0)
    {
        close(ftp_info->sock);
        return 1;
    }

    printf("COPY DONE\n");

    info->total_bytes_recv = 0;
    char md5_buffer[50];
    int md5_bytes = 0;
    while (md5_bytes <= 0)
    {
        if ((md5_bytes = recv(info->sock, md5_buffer, 50, 0)) <= 0) 
            md5_bytes = -1;
        printf("bytes recieved: %i\n", info->bytes_recv);
    }
    printf("Recieved: %s\n", md5_buffer);


    close(ftp_info->sock);
    free(ftp_info);
    return 0;
}

int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s  [<Echo Port>]\n",
                argv[0]);
        exit(1);
    }

    serv_info * info = new serv_info;
    info->server_port = atoi(argv[1]); /* set port */

    const int USER_INPUT_MAX = 256;
    char user_input[USER_INPUT_MAX];

    printf("Enter Message: ");
    fgets(user_input, USER_INPUT_MAX, stdin);
    user_input[strlen(user_input)-1] = '\0';


    info->command_string = user_input;

    if(startup(info) != 0)
        exit(1);

    send_req(info);

    close(info->sock);

    return 0;
}
