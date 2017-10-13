#include "./lib/unity/unity.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "./src/nameserver.h"
#include "./src/encode.h"
#include "./src/encode.c"

using std::string;
using std::cout;
using std::endl;
using std::cerr;

static int port = 50000;

/* test used to ensure testing framework is working properly */
void sanity_test() {
  int a = 1;
  TEST_ASSERT( a == 1 ); //this one will pass
}

void clinet_test1()
{
  const int SERVICE_PORT = port;	/* hard-coded port number TODO pass as commmand line arg via -p */
  const int BUFLEN = 2048;


  struct sockaddr_in myaddr, remaddr;
	int fd, slen=sizeof(remaddr);
	string server = "127.0.0.1";	/* change this to use a different server */
	char buf[BUFLEN];

  request_t* my_request = new request_t();
  my_request->msg_type = DEFINE_PORT;
  strncpy(my_request->service_name, "woo.com", sizeof(my_request->service_name));
  my_request->port = 30;

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
	if (inet_aton(server.c_str(), &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	/* send messages */

	printf("Sending packet to port %d\n", SERVICE_PORT);
	sprintf(buf, "This is a packet");

  /* create & encode request_t to be sent to the ns */
  void* buffer;
  buffer = encode(my_request, buffer);


	if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&remaddr, slen)==-1)
    perror("sendto");


	close(fd);

}

int main(int argc, char** argv) {



  UNITY_BEGIN();
    RUN_TEST(sanity_test);
    RUN_TEST(clinet_test1);
  return UNITY_END();
  // return 0;
}

/*TODO ask phil if we need to validate the args */
//  bool error = false;
//  /* check if valid args */
//  if(!isdigit(service_port)) error = true;
//  cout << error << endl;
//  if(!isdigit(min_supported_ports)) error = true;
//  cout << error << endl;
//  if(!isdigit(keep_alive_time)) error = true;
//  cout << error << endl;
 //
//  cout << "service_port: " << service_port << endl;
//  cout << "min_supported_ports: " << min_supported_ports << endl;
//  cout << "keep_alive_time: " << keep_alive_time << endl;
 //
//  /* if invalid arg is passed, print error and terminate */
//  if(error) cerr << "invalid arg passed, terminating" << endl;
