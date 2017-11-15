// ******************************************************

// file transfer server
//
// Author: Zachary Wentworth
// Email:  zachary.wentworth@oit.edu
//
// ******************************************************

/* system lib includes */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <signal.h>

/* file includes */
#include "getport.h"
#include "md5sum.h"
#include "timedaccept.c"

/* defines */
#define MAX_QUEUE 5    /* max listen queue */
#define RCVBUFSIZE 256 /* Size of receive buffer */

/* using */
using std::string;
string tranfer_service_name = "ftp_zacharywentworth";
static std::atomic<int> transfer_service_counter;
static std::atomic<int> service_names_counter;
int g_time_accept = 50;

bool server_shutdown = false;

static void sig_handler(int signal)
{
    fprintf(stderr, "Received broken pipe signal\n");
    transfer_service_counter--;
}

template<class Container>
void split_string(const std::string& str, Container& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

int get_port_from_ns(string service_name) {
    const string LOCAL_HOST = "127.0.0.1"; /* IP of ns */
    const int    NS_PORT    = 50050;       /* port of NS */

    int setup_ret_code = 0;
    int server_port    = 0;

    if ((setup_ret_code = setup_ns(LOCAL_HOST.c_str(), NS_PORT)) < 0) return -1;

    if ((server_port = request_port(service_name.c_str())) < 0) {
        printf("port recieved: %i\n", server_port);
        return -1;
    }

    printf("Recieving requests on port: %i\n", server_port);

    return server_port;
}

/* gets the names of files in a directory */
std::vector<string>get_file_names_in_dir(string dir_name) {
    DIR *d;
    struct dirent *dir;

    d = opendir(dir_name.c_str());

    std::vector<std::string> files_in_dir;

    /* check if dir exists */
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            /* if of the following extensions, .c .cpp .h .txt
             * add to the list of files in dir */
            std::vector<std::string> split_file_name;
            string file_name(dir->d_name);
            split_string(file_name, split_file_name, '.');

            /* check file type, before pushing to list of files */
            if ((split_file_name.back() == "c") ||
                (split_file_name.back() == "cpp") ||
                (split_file_name.back() == "h") ||
                (split_file_name.back() == "txt")) {
                files_in_dir.push_back(file_name);
            }
        }
        closedir(d);
    }

    return files_in_dir;
}

int send_file_to_client(string dir_name,
                        string file_name,
                        int    ft_port,
                        int    client_com_sock) {
    /* setup for socket for file transfer ***********************************/
    int ret_code;                   /* holds return code of sys calls */
    int transfer_sock_fd;           /* file descriptor for socket */
    struct sockaddr_in addr_bind;   /* socket parameters for bind */
    struct sockaddr_in addr_accept; /* socket parameters for accept */
    socklen_t addrlen;              /* argument to accept */

    /* create Internet domain socket */
    int file_transfer_sock_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (file_transfer_sock_desc == -1) {
        fprintf(stderr, "unable to create socket: %s\n", strerror(errno));
        return -1;
    }

    /* fill in socket structure */
    memset(&addr_bind, 0, sizeof(addr_bind));
    addr_bind.sin_family      = AF_INET;
    addr_bind.sin_addr.s_addr = INADDR_ANY;
    addr_bind.sin_port        = htons(ft_port);

    /* allow re-use of port */
    /* NOTE will fail to bind without this */
    int yes = 1;

    if (setsockopt(file_transfer_sock_desc, SOL_SOCKET, SO_REUSEADDR,
                   &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt");
        return -1;
    }

    /* bind socket to the port */
    ret_code = bind(file_transfer_sock_desc,
                    (struct sockaddr *)&addr_bind,
                    sizeof(addr_bind));

    if (ret_code == -1) {
        fprintf(stderr, "unable to bind to socket: %s\n", strerror(errno));
        return -1;
    }

    /* listen for clients on the socket */
    ret_code = listen(file_transfer_sock_desc, 1);

    if (ret_code == -1) {
        fprintf(stderr, "listen failed: %s\n", strerror(errno));
        return -1;
    }

    /* end of socket set up *************************************************/

    /* send file: <filename> <port> message *********************************/
    string send_buffer = "file: " + file_name + " " +
                         std::to_string(ft_port) + "\n";

    printf("sending: %s\n", send_buffer.c_str());

    int send_size = send_buffer.size();

    if (send(client_com_sock, send_buffer.c_str(), send_size,
             0) !=
        send_size) fprintf(stderr, "send() failed: %s\n", strerror(errno));


    if ((transfer_sock_fd = timed_accept(file_transfer_sock_desc,
                                         (struct sockaddr *)&addr_accept,
                                         &addrlen, 50)) <= 0)
    {
        fprintf(stderr, "accept failed: \n");
        return -1;
    }

    /* end send file: <filename> <port> message *******************************/

    /* send file **************************************************************/
    const int SEND_BUFFER = 50;
    char   buffer[SEND_BUFFER];
    string file_to_open = dir_name + file_name;
    FILE  *file         = fopen(file_to_open.c_str(), "rb");

    if (file == NULL) return -1;

    int left = 0;
    printf("sending file: %s\n", file_to_open.c_str());

    if (file) {
        while ((left = fread(buffer, 1, SEND_BUFFER, file)) == SEND_BUFFER) {
            if (send(transfer_sock_fd, buffer, SEND_BUFFER,
                     0) != SEND_BUFFER) return -1;
        }

        if (send(transfer_sock_fd, buffer, left, 0) != left) return -1;
    }

    /* end send file *********************************************************/

    /* Close everything */
    fclose(file);
    close(transfer_sock_fd);
    close(file_transfer_sock_desc);

    /* send md5checksum *********************************/
    char md5_buff[33];
    char compatibility_buffer[100];
    strcpy(compatibility_buffer, file_to_open.c_str());

    if (CalcFileMD5(compatibility_buffer, md5_buff) == 0) return -1;

    char md5_message[45] = "md5sum: ";
    strcat(md5_message, md5_buff);
    strcat(md5_message, "\n");
    printf("sending md5sum: %s", md5_message);
    send(client_com_sock, md5_message, 41, 0);

    /* end send md5 *************************************/

    return 0;
}

/* TCP client handling function */
void HandleTCPClient(int client_sock, string service_name, int ft_port)
{
    char echoBuffer[RCVBUFSIZE] = { 0 }; /* Buffer for echo string */
    int  recvMsgSize;                    /* Size of received message */

    /* Receive message from client */
    if ((recvMsgSize = recv(client_sock, echoBuffer, RCVBUFSIZE, 0)) < 0) {
        fprintf(stderr, "recv() failed: %s\n", strerror(errno));
        close(client_sock); /* Close client socket */
        /* delete sock dec counter */
        transfer_service_counter--;
    }

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0) /* zero indicates end of transmission */
    {
        printf("received from client: %s\n", echoBuffer);

        /* parse message from the client */
        std::vector<std::string> split_echo_strings;
        split_echo_strings.clear();
        split_string(echoBuffer, split_echo_strings);

        /* get files request */
        if ((split_echo_strings[0] == "get") &&
            (split_echo_strings.size() == 2)) {
            string dir_to_get = split_echo_strings[1];
            printf("%s%s%s\n",
                   "get file request recieved for: ",
                   dir_to_get.c_str(),
                   " preparing to send...");

            /* get list of files to get */
            std::vector<std::string> files_to_send;
            files_to_send = get_file_names_in_dir(dir_to_get);

            if (files_to_send.empty()) {
                /* send error */
                printf("%s\n",
                       "invalid cmd recieved, sending error message...");
                char sendBuffer[RCVBUFSIZE] =
            "error: either the requested dir doesn't exist or it is empty\n";
                int send_size = sizeof(sendBuffer);

                /* Echo message back to client */
                if (send(client_sock, sendBuffer, send_size,
                         0) != send_size) fprintf(stderr,
                                                  "send() failed: %s\n",
                                                  strerror(errno));
            }

            /* ensure placement of '/' at end of dir & check if abs dir */
            dir_to_get += '/';

            /* operation on files in dir */
            for (std::vector<string>::size_type i =
                     0; i != files_to_send.size();
                 i++) {
                send_file_to_client(dir_to_get,
                                    files_to_send[i],
                                    ft_port,
                                    client_sock);

                /* check if end, if so send done message */
                if (i + 1 == files_to_send.size()) {
                    printf("%s\n", "done sending files...");
                    string send_buffer = "done\n";
                    int    send_size   = send_buffer.size();

                    /* Echo message back to client */
                    if (send(client_sock,
                             send_buffer.c_str(),
                             send_size, 0) != send_size)
                    {
                        fprintf(stderr, "send() failed: %s\n", strerror(errno));
                    }
                }
                /* clear port */
                release_port(service_name.c_str(), ft_port);
            }
        }

        /* exit request */
        else if (split_echo_strings[0] == "exit") {
            printf("%s\n", "exit recieved, kill client...");
            close(client_sock); /* Close client socket */
            transfer_service_counter--;
            return;
        }

        /* shutdown */
        else if (split_echo_strings[0] == "shutdown") {
            printf("%s\n", "shutdown recieved, starting shutdown process...");
            close(client_sock); /* Close client socket */
            server_shutdown = true;

            /* set counter amount so no more clients will be accepted */
            transfer_service_counter = 100;
            return;
        }
        else {
            printf("%s\n", "invalid cmd recieved, sending error message...");
            char sendBuffer[RCVBUFSIZE] = "error: invalid request\n";
            int  send_size              = sizeof(sendBuffer);

            /* Echo message back to client */
            if (send(client_sock, sendBuffer, send_size,
                     0) !=
                send_size) fprintf(stderr, "send() failed: %s\n", strerror(
                                       errno));
        }

        /* clear out client message buffer */
        memset(echoBuffer, 0, sizeof(echoBuffer));

        /* See if there is more data to receive */
        if ((recvMsgSize = recv(client_sock, echoBuffer, RCVBUFSIZE, 0)) < 0) {
            fprintf(stderr, "recv() failed: %s\n", strerror(errno));
            close(client_sock); /* Close client socket */
            transfer_service_counter--;
        }
    }
}

int main(int argc, char **argv) {
    /* for client crashing */
    if (signal(SIGPIPE, sig_handler) == SIG_ERR)
    {
        fprintf(stderr, "Unable to set up signal hander. Subject to crashes\n");
    }

    string service_name = "ftp_zacharywentworth"; /* service_name of server */

    int server_port = 0;                     /* port number to use */
    int sock_desc;                           /* socket desciptor */
    struct sockaddr_in addr_bind;            /* socket parameters for bind */
    struct sockaddr_in addr_accept;          /* socket parameters for accept */
    socklen_t addrlen;                       /* argument to accept */
    int       ret_code;                      /* holds return code of sys calls
                                                */
    const int KEEP_ALIVE = 3;

    /* get port from ns */
    server_port = get_port_from_ns(service_name);

    if (server_port == -1) {
        fprintf(stderr, "get port from NS failed\n");
        exit(1);
    }

    /* create Internet domain socket */
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_desc == -1) {
        fprintf(stderr, "unable to create socket: %s\n", strerror(errno));
        exit(1);
    }

    /* fill in socket structure */
    memset(&addr_bind, 0, sizeof(addr_bind));
    addr_bind.sin_family      = AF_INET;
    addr_bind.sin_addr.s_addr = INADDR_ANY;
    addr_bind.sin_port        = htons(server_port);

    /* bind socket to the port */
    ret_code = bind(sock_desc,
                    (struct sockaddr *)&addr_bind,
                    sizeof(addr_bind));

    if (ret_code == -1) {
        fprintf(stderr, "unable to bind to socket: %s\n", strerror(errno));
        exit(1);
    }

    /* listen for clients on the socket */
    ret_code = listen(sock_desc, 10);

    if (ret_code == -1) {
        fprintf(stderr, "listen failed: %s\n", strerror(errno));
        exit(1);
    }

    while (!server_shutdown) {
        if (transfer_service_counter < 5) {
            /* wait for a client to connect */
            int *sock_fd = new int();

            /* send keep alive signal */
            int keep_alive_resp =
                do_request(service_name.c_str(), KEEP_ALIVE, server_port);

            if (keep_alive_resp < 0) {
                fprintf(stderr, "keep alive failure\n");
            }

            *sock_fd = timed_accept(sock_desc,
                                    (struct sockaddr *)&addr_accept,
                                    &addrlen, g_time_accept);

            if (*sock_fd == -1) {
                fprintf(stderr, "accept failed: %s\n", strerror(errno));
            } else if (!server_shutdown) {
                printf("current number of clients: %i\n",
                       (int)transfer_service_counter + 1);

                /* increment counter*/
                transfer_service_counter++;

                /* get port to use for file_transfer */
                std::string service_name = tranfer_service_name +
                                          std::to_string(service_names_counter);

                service_names_counter++;

                int ft_port = get_port_from_ns(service_name);

                if (ft_port == -1) {
                    fprintf(stderr, "get port from NS failed\n");
                    transfer_service_counter--;
                }

                std::thread client_thread(HandleTCPClient,
                                          *sock_fd,
                                          service_name,
                                          ft_port);
                client_thread.detach();
            }
            delete(sock_fd);
        }
    }
    close(sock_desc);
    release_port(service_name.c_str(), server_port);
}
