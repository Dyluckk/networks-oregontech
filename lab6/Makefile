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

dns_resolver: dns_resolver.cpp dns_header.h
	g++ $(COPTS) $(CFLAGS) dns_resolver.cpp -o dns_resolver

dns_client: dns_client.cpp
	g++ $(COPTS) $(CFLAGS) dns_client.cpp -o dns_client
