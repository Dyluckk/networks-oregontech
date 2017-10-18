/******************************************************************************
 *
 *
 *
 *
 *
 *
 *
//TODO check if port is okay in keep_alive
//TODO check if port is zero
//TODO Ensure that status passed in is SUCCESS
//TODO check if port is the same in remove
//TODO return PORT 0 on bad requests
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

#include "ns_handler.h"

using std::cout;
using std::endl;
using std::string;

#define BUFSIZE 2048

string get_current_time() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d-%X", &tstruct);

    return buf;
}

void log_incoming_request(request_t* request_buff) {
    string msg_type_string;
    string status_string;

    /* generate string version of response codes for better readability */
    if(request_buff->msg_type == DEFINE_PORT) msg_type_string = "DEFINE_PORT";
    if(request_buff->msg_type == LOOKUP_PORT) msg_type_string = "LOOKUP_PORT";
    if(request_buff->msg_type == KEEP_ALIVE) msg_type_string = "KEEP_ALIVE";
    if(request_buff->msg_type == CLOSE_PORT) msg_type_string = "CLOSE_PORT";
    if(request_buff->msg_type == RESPONSE) msg_type_string = "RESPONSE";
    if(request_buff->msg_type == STOP) msg_type_string = "STOP";

    if(request_buff->status == SUCCESS) status_string = "SUCCESS";
    if(request_buff->status == SERVICE_IN_USE) status_string = "SERVICE_IN_USE";
    if(request_buff->status == SERVICE_NOT_FOUND) status_string = "SERVICE_NOT_FOUND";
    if(request_buff->status == ALL_PORTS_BUSY) status_string = "ALL_PORTS_BUSY";
    if(request_buff->status == INVALID_ARG) status_string = "INVALID_ARG";
    if(request_buff->status == UNDEFINED_ERROR) status_string = "UNDEFINED_ERROR";

    fprintf(stdout,"%s\n","====================================================" );
    fprintf(stdout,"%s\n","====================================================" );
    fprintf(stdout,"%s\n","====================================================" );
    fprintf(stdout,"%s\n","***************** RECIEVED REQUEST *****************" );
    fprintf(stdout, "time of request: %s\n", get_current_time().c_str());
    fprintf(stdout,"msg_type: \"%s\"\n", msg_type_string.c_str());
    fprintf(stdout,"status: \"%s\"\n", status_string.c_str());
    fprintf(stdout,"service_name: \"%s\"\n", request_buff->service_name);
    fprintf(stdout,"port: \"%d\"\n", request_buff->port);
}

void log_outgoing_response(request_t* response_buff) {
    string msg_type_string;
    string status_string;

    /* generate string version of response codes for better readability */
    if(response_buff->msg_type == DEFINE_PORT) msg_type_string = "DEFINE_PORT";
    if(response_buff->msg_type == LOOKUP_PORT) msg_type_string = "LOOKUP_PORT";
    if(response_buff->msg_type == KEEP_ALIVE) msg_type_string = "KEEP_ALIVE";
    if(response_buff->msg_type == CLOSE_PORT) msg_type_string = "CLOSE_PORT";
    if(response_buff->msg_type == RESPONSE) msg_type_string = "RESPONSE";
    if(response_buff->msg_type == STOP) msg_type_string = "STOP";

    if(response_buff->status == SUCCESS) status_string = "SUCCESS";
    if(response_buff->status == SERVICE_IN_USE) status_string = "SERVICE_IN_USE";
    if(response_buff->status == SERVICE_NOT_FOUND) status_string = "SERVICE_NOT_FOUND";
    if(response_buff->status == ALL_PORTS_BUSY) status_string = "ALL_PORTS_BUSY";
    if(response_buff->status == INVALID_ARG) status_string = "INVALID_ARG";
    if(response_buff->status == UNDEFINED_ERROR) status_string = "UNDEFINED_ERROR";

    printf("%s\n","***************** SENDING RESPONSE *****************" );
    fprintf(stdout, "time of request: %s\n", get_current_time().c_str());
    fprintf(stdout,"msg_type: \"%s\"\n", msg_type_string.c_str());
    fprintf(stdout,"status: \"%s\"\n", status_string.c_str());
    fprintf(stdout,"service_name: \"%s\"\n", response_buff->service_name);
    fprintf(stdout,"port: \"%d\"\n", response_buff->port);
}

void log_bad_request() {
    fprintf(stdout,"%s\n","====================================================" );
    fprintf(stdout,"%s\n","====================================================" );
    fprintf(stdout,"%s\n","====================================================" );
    printf("%s\n","@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );
    printf("%s\n","@@@@@@@@@@@@@@@@@@   BAD REQUEST   @@@@@@@@@@@@@@@@@" );
    fprintf(stdout, "%s%s%s\n", "@@@@@@@@@@@@@@@ ", get_current_time().c_str() ," @@@@@@@@@@@@@@@@");
    printf("%s\n","@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );
}

void log_start_up() {
  printf("%s\n","@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );
  printf("%s\n","@@@@@@@@@@@@@@@@@@   STARTUP...   @@@@@@@@@@@@@@@@@@" );
  fprintf(stdout, "%s%s%s\n", "@@@@@@@@@@@@@@@ ", get_current_time().c_str() ," @@@@@@@@@@@@@@@@");
  printf("%s\n","@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );
}

void log_shutdown() {
  printf("%s\n","@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );
  printf("%s\n","@@@@@@@@@@@@@@@@@@   SHUTDOWN...   @@@@@@@@@@@@@@@@@" );
  fprintf(stdout, "%s%s%s\n", "@@@@@@@@@@@@@@@ ", get_current_time().c_str() ," @@@@@@@@@@@@@@@@");
  printf("%s\n","@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );
}

void print_help_message() {
  cout << "Available arguements for nameserver tester:" << endl;
  cout << "-------------------------------------------" << endl;
  cout << "-p <service port of nameserver>\n" <<
          "-n <minimum number of supported ports>\n" <<
          "-t <keep alive time in seconds>\n" <<
          "-h *prints help message*\n" <<
          "NOTE: ALL VALUES MUST BE INTEGERS" <<endl;
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
string get_log_file_name() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "./_logs/%Y-%m-%d-%X.log", &tstruct);

    return buf;
}

/* function used to forward actions based on request_t->msg_type */
request_t* generate_response(request_t* decoded_request, ns_lookup_table& lookup_table, int keep_alive_time, unsigned int max_supported_ports) {
  if (decoded_request->msg_type == DEFINE_PORT) {
    /* check if full */
    if(lookup_table.size() == max_supported_ports) {
        clear_timedout_services(lookup_table, keep_alive_time);
    }
    /* check if space avail after clear was made */
    if(lookup_table.size() < max_supported_ports) {
        decoded_request = define_service(decoded_request, lookup_table, keep_alive_time);
    } else {
        decoded_request = create_ports_full_response(decoded_request);
    }
  } else if (decoded_request->msg_type == LOOKUP_PORT) {
    decoded_request = lookup_service_port(decoded_request, lookup_table);
  } else if (decoded_request->msg_type == KEEP_ALIVE) {
      /* check if full */
      if(lookup_table.size() == max_supported_ports) {
          clear_timedout_services(lookup_table, keep_alive_time);
      }
      /* check if space avail after clear was made */
      if(lookup_table.size() < max_supported_ports) {
          decoded_request = keep_alive(decoded_request, lookup_table, keep_alive_time);
      } else {
          decoded_request = create_ports_full_response(decoded_request);
      }
  } else if (decoded_request->msg_type == CLOSE_PORT) {
    decoded_request = remove_service(decoded_request, lookup_table);
  } else if (decoded_request->msg_type == STOP) {
    decoded_request = create_shutdown_request_response(decoded_request);
  }

  return decoded_request;
}

int main(int argc, char **argv) {
    /* get name of session log file */
    string log_file_name = get_log_file_name();
    // string log_file_name = "/dev/tty";
    int option = 0;
    /*
     * give defaults values for options
     * -p 50000 –n 100–t 300
     */
    int service_port = 50000;
    unsigned int max_supported_ports = 100;
    int keep_alive_time = 300;
    ns_lookup_table lookup_table;

    /* check for the following command line args
    * -h (print help, "tell user what args do what")
    * -p <service port>
    * -n <minimum number of supported ports>
    * -t <keep alive time in seconds>
    */
    while ((option = getopt(argc, argv,"hp:n:t:")) != -1) {
        switch (option) {
             case 'h' : print_help_message(); return 0;
                 break;
             case 'p' : service_port = atoi(optarg);
                 break;
             case 'n' : max_supported_ports = atoi(optarg);
                 break;
             case 't' : keep_alive_time = atoi(optarg);
                 break;
        }
    }

	int fd;	/* our socket */
    const int NS_PORT = service_port; /* port the nameserver is sitting on */
    unsigned int address_length; /* length of address (for getsockname) */
    struct sockaddr_in ns_address; /* our address (a sockaddr container) */
    struct sockaddr_in remaddr; /* remote address */
    int slen = sizeof(remaddr);
    int recvlen; /* # of bytes that were read into buffer */

	/* create a udp/ip socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}

    /* fill out the sockaddr_in structure */
    memset((void *)&ns_address, 0, sizeof(ns_address));
    ns_address.sin_family = AF_INET;/* set address family */
    ns_address.sin_addr.s_addr = htonl(INADDR_ANY); /* set the address for the socket */
    ns_address.sin_port = htons(NS_PORT); /* set the transport address (port #) */

    /* bind to an arbitrary return address */
    if (bind(fd, (struct sockaddr *)&ns_address, sizeof(ns_address)) < 0) {
		perror("bind failed");
		return 0;
	}

	address_length = sizeof(ns_address);
	if (getsockname(fd, (struct sockaddr *)&ns_address, &address_length) < 0) {
		perror("getsockname failed");
		return 0;
	}

    /* Run forever */
    printf("waiting on port %d\n", NS_PORT);

    freopen (log_file_name.c_str(),"a+",stdout);
    printf("NS WAS LISTENING ON PORT: %d\n", NS_PORT);
    log_start_up();
    fclose (stdout);

    /* used for when a STOP msg_type is sent to the server */
    bool turn_off_server = false;
    while(!turn_off_server) {
        request_t * client_buff = (request_t*)malloc(sizeof(request_t));
        request_t * response_buff = (request_t*)malloc(sizeof(request_t));
        recvlen = recvfrom(fd, client_buff, sizeof(request_t), 0, (struct sockaddr *)&remaddr, &address_length);

        /* ensure bytes were recieved */
            if (recvlen > 0) {

                /* decode request */
                response_buff = decode(response_buff, client_buff);

                /* check if request was valid before process (on fail decode returns NULL) */
                if(response_buff) {

                    bool stop_triggered = false;
                    if(response_buff->msg_type == STOP) {
                        stop_triggered = true;
                    }

                    /* print to log */
                    freopen (log_file_name.c_str(),"a+",stdout);
                    log_incoming_request(response_buff);
                    fclose (stdout);

                    /* process */
                    freopen (log_file_name.c_str(),"a+",stdout);
                    response_buff = generate_response(response_buff, lookup_table, keep_alive_time, max_supported_ports);
                    fclose (stdout);
                    
                    /* encode response */
                    void* encoded_response = encode(response_buff, response_buff);

                    /* print to log */
                    freopen (log_file_name.c_str(),"a+",stdout);
                    log_outgoing_response(((request_t*)encoded_response));
                    fclose (stdout);

                    /* respond to client */
                    if (sendto(fd, encoded_response, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
                        perror("sendto");

                    /* check if a shutdown request was sent */
                    if(stop_triggered) {
                        turn_off_server = true;
                        /* print shutdown to the log_file */
                        freopen (log_file_name.c_str(),"a+",stdout);
                        log_shutdown();
                        fclose (stdout);
                    }


                } else {
                    /* pring to log */
                    freopen (log_file_name.c_str(),"a+",stdout);
                    log_bad_request();
                    fclose (stdout);

                    /* create bad request response */
                    client_buff = create_bad_request_response(client_buff);
                    /* encode response */
                    void* encoded_response = encode(client_buff, client_buff);

                    /* print to log */
                    freopen (log_file_name.c_str(),"a+",stdout);
                    log_outgoing_response(((request_t*)encoded_response));
                    fclose (stdout);

                    /* respond to client */
                    if (sendto(fd, encoded_response, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
                        perror("sendto");
                }
            } else {
                /* pring to log */
                freopen (log_file_name.c_str(),"a+",stdout);
                log_bad_request();
                fclose (stdout);

                /* create bade  request response */
                client_buff = create_bad_request_response(client_buff);
                /* encode response */
                void* encoded_response = encode(client_buff, client_buff);

                /* print to log */
                freopen (log_file_name.c_str(),"a+",stdout);
                log_outgoing_response(((request_t*)encoded_response));
                fclose (stdout);

                /* respond to client */
                if (sendto(fd, encoded_response, sizeof(request_t), 0, (struct sockaddr *)&remaddr, slen)==-1)
                    perror("sendto");
            }
        free(response_buff);
        free(client_buff);
    }
    return 0;
}
