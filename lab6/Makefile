#**************************************************
# Makefile for DNS resolver and test client
#**************************************************
COPTS = -g -O0 -Wall -pthread
CFLAGS := $(CFLAGS) -std=c++11 -lresolv

all: dns_resolver\
	 dns_client\

clean:
	rm -f dns_resolver
	rm -f dns_client
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

dns_resolver: dns_resolver.cpp dns_header.h Locked_Multi.h Locked_Map.h timed_accept.h timed_recvfrom.h
	g++ $(COPTS) $(CFLAGS) dns_resolver.cpp Locked_Multi.cpp Locked_Map.cpp timed_accept.c timed_recvfrom.c -o dns_resolver

dns_client: dns_client.cpp
	g++ $(COPTS) $(CFLAGS) dns_client.cpp -o dns_client
