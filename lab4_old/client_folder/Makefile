#**************************************************
# Makefile for OIT Nameserver lab
# CST 415 Lab 2
#
#
COPTS = -g -O0 -Wall -pthread
CFLAGS := $(CFLAGS) -std=c++11

all: simple_client\


clean:
	rm -f simple_client
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

simple_client: simple_client.cpp 
	g++ $(COPTS) $(CFLAGS) simple_client.cpp -o simple_client

