#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "getport.h"
#include "md5sum.h"
#include "timedaccept.c"
#include "tscounter.h"

#define MAXPENDING 5
#define BUFFER 50

using std::string;

const string ft_service = "ft_service";
const string ftp_service_name = "ftp_zacharyw";

typedef struct
{
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int server_port;
    unsigned int client_len;
} server_settings;

typedef struct
{
    int comm_port;
    int ftp_port;
    unsigned long int count;
} connection_info;

void DieWithError(const char * errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int ready_and_bind(server_settings * socket_settings)
{
    if((socket_settings->server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        return 1;

    memset(&(socket_settings->server_addr), 0, sizeof(socket_settings->server_addr));

    socket_settings->server_addr.sin_family = AF_INET;
    socket_settings->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_settings->server_addr.sin_port = htons(socket_settings->server_port);

    //bind
    if(bind(socket_settings->server_sock, (struct sockaddr *) &socket_settings->server_addr,
                sizeof(socket_settings->server_addr)) < 0)
        return 2;

    return 0;
}

int send_file(string transfer_file, connection_info info)
{
    string file_port = "ftp_server_file:" + std::to_string(info.count);

    string file_info = "file: " + transfer_file + " " + std::to_string(info.ftp_port) + "\n";
    if(info.ftp_port < 0)
        return 4;

    char buffer[BUFFER];
    FILE * file = fopen(transfer_file.c_str(), "rb");
    if(file == NULL)
        return 3;

    server_settings * file_transfer = (server_settings*)malloc(sizeof(server_settings));
    file_transfer->server_port = info.ftp_port;

    if(ready_and_bind(file_transfer) != 0)
        return 1;

    if(listen(file_transfer->server_sock, MAXPENDING) < 0)
        return 3;

    file_transfer->client_len = sizeof(file_transfer->client_addr);

    send(info.comm_port, file_info.c_str(), file_info.length(), 0);

    printf("FTP Port: %i\n", info.ftp_port);
    if ((file_transfer->client_sock = timed_accept(file_transfer->server_sock,
                    (struct sockaddr *) &file_transfer->client_addr,
                    &file_transfer->client_len, 50)) <= 0)
        DieWithError("Failed to accept");

    int left = 0;
    if(file)
    {
        while((left = fread(buffer, 1, BUFFER, file)) == BUFFER)
        {
            if (send(file_transfer->client_sock, buffer, BUFFER, 0) != BUFFER)
                return 6;
        }
        if (send(file_transfer->client_sock, buffer, left, 0) != left)
            return 7;
    }

    char md5_buff[33];
    char compatibility_buffer[100];
    strcpy(compatibility_buffer, transfer_file.c_str());
    if(CalcFileMD5(compatibility_buffer, md5_buff) == 0)
        return 4;

    char md5_message[40] = "md5sum ";
    strcat(md5_message, md5_buff);
    printf("sending md5sum: %s\n", md5_message);
    strcat(md5_message, "\n");
    send(info.comm_port, md5_message, 40, 0);
    close(file_transfer->client_sock);
    free(file_transfer);

    return 0;

}

int handle_get(string directory, connection_info info)
{
    //for file in dir
    //
    int result = send_file(directory, info);
    printf("service name release: %s\n", string(ftp_service_name + std::to_string(info.count)).c_str());

    send(info.comm_port, "done", 4, 0);
    return result;
}


int HandleTCPClient(connection_info info)
{
    char echo_buff[BUFFER] = {0};
    int msg_size;
    int req_result = 1;

    do
    {
        if ((msg_size = recv(info.comm_port, echo_buff, BUFFER, 0)) < 0)
            return 1;

        string service_name = string(ftp_service_name + std::to_string(info.count));
        string request = string(echo_buff);
        if(request.substr(0, 3) == "get")
        {
            req_result = handle_get(request.substr(4), info);
        }
        if(request.substr(0, 4) == "exit")
        {
            int release = -1;
            if((release = release_port(string(ftp_service_name +
                                std::to_string(info.count)).c_str(), info.ftp_port)) < 0)
            {
                printf("Result of release_port: %i\n", release);
                return 1;
            }
            close(info.comm_port);
            close(info.ftp_port);
            break;
        }
        if(request.substr(0, 8) == "shutdown")
        {
            //clean shutdown. may need to check earlier
        }

        printf("size of echoBuffer: %s\n", echo_buff);
    } while (msg_size > 0);

    return req_result;
}


int start_server(server_settings * srv_info)
{
    //TODO get port from nameserver
    int setupns = 0;
    if((setupns = setup_ns("127.0.0.1", 50050)) < 0)
        return 1;

    printf("About to request port\n");
    if((srv_info->server_port = request_port(ftp_service_name.c_str())) < 0)
    {
        printf("port recieved: %i\n", srv_info->server_port);
        return 2;
    }

    printf("Recieving requests on port: %i\n", srv_info->server_port);

    if(ready_and_bind(srv_info) != 0)
        return 1;

    if(listen(srv_info->server_sock, MAXPENDING) < 0)
        return 3;
    return 0;
}

int wait_for_client(server_settings * srv_info)
{
    unsigned long int count = 0;
    int ftp_port = -1;
    connection_info connection;

    while(1)
    {
        ++count;

        srv_info->client_len = sizeof(srv_info->client_addr);

        int ftp_port = request_port(string(ftp_service_name + std::to_string(count)).c_str());
        if(ftp_port < 0)
            return 1;

        if((srv_info->client_sock = accept(srv_info->server_sock, (struct sockaddr *) &srv_info->client_addr,
                        &srv_info->client_len)) < 0)
            return 2;

        connection.count = count;
        connection.ftp_port = ftp_port;
        connection.comm_port = srv_info->client_sock;
        printf("Client Sock: %i\n", srv_info->client_sock);
        printf("Comm Port: %i\n", connection.comm_port);

        HandleTCPClient(connection);
    }

    if(release_port(ftp_service_name.c_str(), ftp_port) < 0)
        return 3;

    return 0;
}

int main(int argc, char *argv[])
{

    server_settings * srv_info = (server_settings*)malloc(sizeof(server_settings));
    if(start_server(srv_info) != 0)
    {
        free(srv_info);
        return 1;
    }

    //TODO switch case for args
    //TODO ask for port on NS

    int result = wait_for_client(srv_info);
    free(srv_info);
    return result;
}
