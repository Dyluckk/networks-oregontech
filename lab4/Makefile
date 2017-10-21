#**************************************************
# Makefile for OIT Nameserver lab
# CST 415 Lab 2
#
# Author: Zachary Wentworth
# Email:  zachary.wentworth@oit.edu
#
COPTS = -g -O0 -Wall -pthread
CFLAGS := $(CFLAGS) -std=c++11

all: server\
	simple_client\


clean:
	rm -f server
	rm -f simple_client
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

server: server.cpp
	g++ $(COPTS) $(CFLAGS) server.cpp -o server

simple_client: simple_client.cpp
	g++ $(COPTS) $(CFLAGS) simple_client.cpp -o simple_client
