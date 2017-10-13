/******************************************************************************
 *
 *
 *
 *
 *
 *
 * //TODO need to write func for after exceeding min_supported_ports, clean up a timedout service and replace
 * //TODO if even after cleanup still full respond with ALL_PORTS_BUSY
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "ns_handler.h"

#define BUFSIZE 2048

void print_help_message() {
  cout << "Available arguements for nameserver tester:" << endl;
  cout << "-------------------------------------------" << endl;
  cout << "-p <service port of nameserver>\n" <<
          "-n <minimum number of supported ports>\n" <<
          "-t <keep alive time in seconds>\n" <<
          "-h *prints help message*\n" <<
          "NOTE: ALL VALUES MUST BE INTEGERS" <<endl;
}

/* shut down serice, ensure no memory leaks */
void shutdown() {
  exit(0);
}

/* function used to forward actions based on request_t->msg_type */
request_t* generate_response(request_t* decoded_request, ns_lookup_table& lookup_table,int keep_alive_time) {
  //TODO convert to switch case
  if (decoded_request->msg_type == DEFINE_PORT) {
    //TODO check if full
    decoded_request = define_service(decoded_request, lookup_table, keep_alive_time);
  } else if (decoded_request->msg_type == LOOKUP_PORT) {
    decoded_request = lookup_service_port(decoded_request, lookup_table);
  } else if (decoded_request->msg_type == KEEP_ALIVE) {
    //TODO check if full
    decoded_request = update_service(decoded_request, lookup_table, keep_alive_time);
  } else if (decoded_request->msg_type == CLOSE_PORT) {
    decoded_request = remove_service(decoded_request, lookup_table);
  } else if (decoded_request->msg_type == STOP) {
    shutdown();
  }

  return decoded_request;
}

int main(int argc, char **argv) {

  int option = 0;
  /* give defaults values dor options */
  /* -p 50000 –n 100–t 300            */
  int service_port = 50000;
  int min_supported_ports = 100; // NOTE: n is only needed to prevent denial of service attacks
  int keep_alive_time = 300;

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
         case 'n' : min_supported_ports = atoi(optarg);
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
  unsigned char buf[BUFSIZE]; /* receive buffer */
  int recvlen; /* # of bytes that were read into buffer */

	/* create a udp/ip socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}

	printf("created socket: descriptor = %d\n", fd);

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

	printf("bind complete. Port number = %d\n", ntohs(ns_address.sin_port));

  /* Run forever */
  for (;;) {
    printf("waiting on port %d\n", NS_PORT);
    recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &address_length);
    printf("received %d bytes\n", recvlen);
    /* ensure bytes were recieved */
    if (recvlen > 0) {
      // buf[recvlen] = 0;
      /* handle here */

      /* cast recieved packet to a request_t */
      request_t* tmp_pckt = (request_t*) buf;
      printf("received packet: \"%d\"\n", tmp_pckt->msg_type);
      printf("received packet: \"%d\"\n", tmp_pckt->msg_type);
      printf("received packet: \"%d\"\n", tmp_pckt->msg_type);

      /* decode request */



      /* check if request was valid before process (on fail decode returns NULL) */

      /* process */

      /* encode response */

      /* respond to client */
      // sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);

    }
  }

}


//******************************* testing ***********************************//
//******************************* testing ***********************************//
//******************************* testing ***********************************//
// int main(int argc, char **argv) {
//
//   ns_lookup_table* lookup_table = new ns_lookup_table();
//   int keep_alive_time = 300;
//
//
//   /* test define */
//   request_t* decoded_request;
//   decoded_request->
//
//   /* test keep alive message */
//
//   /* test remove */
//
// }