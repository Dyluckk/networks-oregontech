// Unit test suite for encode and readline
//
// Author: Zachary Wentworth
// Email:  zachary.wentworth@oit.edu

#define MUNIT_ENABLE_ASSERT_ALIASES
#include "./lib/unit/munit.h"

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

#if defined(_MSC_VER)
#pragma warning(disable: 4127)
#endif

static char* port = "1111";

static void*
test_compare_setup(const MunitParameter params[], void* user_data) {
  (void) params;

  munit_assert_string_equal(user_data, "µnit");
  return (void*) (uintptr_t) 0xdeadbeef;
}

static MunitResult
sanity_test() {
  printf("%s\n", port);
  return MUNIT_OK;
}

static MunitResult
basic_client_test() {
  const int SERVICE_PORT = 42733;	/* hard-coded port number TODO pass as commmand line arg via -p */
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
		return 0;
	}

  /* set up the address we want to send messages to */
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	/* send message */
	for (i=0; i < MSGS; i++) {
		printf("Sending packet %d to %s port %d\n", i, server, SERVICE_PORT);
		//  sprintf(buf, "This is packet %d", i);

    /* create & encode request_t to be sent to the ns */

		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1)
			perror("sendto");
	}

	close(fd);
	return MUNIT_OK;
}

/* functions to run */
static MunitTest test_suite_tests[] = {
  { (char*) "SANITY TEST", sanity_test, test_compare_setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "NAMESERVER_TESTS", basic_client_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  /* REQUIRED NULL TERMINATOR */
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite test_suite = {
  (char*) "Lab 2 Unit Tests\n",
  test_suite_tests,
  NULL,
  0,
  MUNIT_SUITE_OPTION_NONE
};

#include <stdlib.h>
#include <fcntl.h>



int main(int argc, char** argv) {
  // port = argv[1];
  // printf("%s\n", argv[1]);
  /* Use µnit here. */
  return munit_suite_main(&test_suite, (void*) argv[1], argc, argv);
}
