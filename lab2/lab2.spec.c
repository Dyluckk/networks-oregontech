#include "./lib/unity/unity.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

static int port = 1234;

/* test used to ensure testing framework is working properly */
void sanity_test() {
  int a = 1;
  TEST_ASSERT( a == 1 ); //this one will pass
}

void clinet_test1()
{
  const int SERVICE_PORT = port;	/* hard-coded port number TODO pass as commmand line arg via -p */
  const int BUFLEN = 2048;
  const int MSGS = 5;	/* number of messages to send */

  struct sockaddr_in myaddr, remaddr;
	int fd, i, slen=sizeof(remaddr);
	char *server = "127.0.0.1";	/* change this to use a different server */
	char buf[BUFLEN];

	/* create a socket */
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	/* bind it to all local addresses and pick any port number */
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return;
	}

  /* set up the address we want to send messages to */
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	/* send messages */
	for (i=0; i < MSGS; i++) {
		// printf("Sending packet %d to %s port %d\n", i, server, SERVICE_PORT);
		//  sprintf(buf, "This is packet %d", i);

    /* create & encode request_t to be sent to the ns */

		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1)
			perror("sendto");
	}

	close(fd);

}

int main(int argc, char** argv) {

  // int c;
  // while ((c = getopt (argc, argv, "p")) != -1)

  /* TODO check for the following command line args
   * -h
   * -p <service port>
   * -n <minimum number of supported ports>
   * -t <keep alive time in seconds>
   */

   // TODO give defaults -p 50000 –n 100–t 300

  if(argv[1]) {
    char *p;
    long conv = strtol(argv[1], &p, 10);

    /* Check to make sure arguemnt passed represents an int */
    if (*p != '\0' || conv > INT_MAX) exit(0);
    port = conv;
  }

  UNITY_BEGIN();
    RUN_TEST(sanity_test);
    RUN_TEST(clinet_test1);
  return UNITY_END();
}
