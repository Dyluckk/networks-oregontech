#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFER 10000
#define RCVBUFSIZE 1024

using std::string;

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

int startup(serv_info * info)
{
    info->server_ip = "127.0.0.1";

    if ((info->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        return 1;

    memset(&info->server_addr, 0, sizeof(info->server_addr));
    info->server_addr.sin_family = AF_INET;

    info->server_addr.sin_addr.s_addr = inet_addr(info->server_ip.c_str());
    info->server_addr.sin_port = htons(info->server_port);

    return 0;
}

int send_req(serv_info * info)
{
    int connection = 0;
    if ((connection = connect(info->sock, (struct sockaddr *)&info->server_addr,
                sizeof(info->server_addr)) < 0))
        perror("Error connecting");

    printf("%s\n", info->command_string.c_str());
    printf("sock: %i\n", info->sock);
    if (send(info->sock, info->command_string.c_str(),
            info->command_string.length() , 0) !=
            (unsigned)info->command_string.length())
        perror("Error sending");

    info->total_bytes_recv = 0;
    memset(info->buffer, 0, BUFFER);
    while (info->bytes_recv <= 0)
    {
        if((info->bytes_recv=recv(info->sock, info->buffer, BUFFER, 0)) <= 0)
            perror("Error receiving");
        printf("response:\n%s\n", info->buffer);
    }

    return 0;
}
int test_server(unsigned short server_port, string message)
{
    serv_info * server = new serv_info;
    server->server_port = server_port;
    if(startup(server) != 0)
    {
        perror("Failed to setup serv_info struct\n");
        return 1;
    }
    server->command_string = message;
    send_req(server);

    return 0;
}

int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        printf("usage: <port> <meesage>\n");
        exit(1);
    }
    else {
        int port = atoi(argv[1]);
        string message = argv[2];

        printf("Port: %i\n", port);
        printf("%i\n", test_server((unsigned short)port, message));
    }
}
