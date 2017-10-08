#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFSIZE 2048

void HandleUDPClient() {

}

int main(int argc, char **argv) {

	int fd;	/* our socket */
  const int NS_PORT = 0; /* port the nameserver is sitting on */
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
      buf[recvlen] = 0;
      /* handle here */
      printf("received message: \"%s\"\n", buf);

      /* decode request */

      /* process */

      /* encode response */

      /* respond to client */
      // sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
    }
  }

}
