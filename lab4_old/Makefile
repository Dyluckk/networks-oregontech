#**************************************************
# Makefile for OIT Nameserver lab
# CST 415 Lab 2
#
#
COPTS = -g -O0 -Wall -pthread
CFLAGS := $(CFLAGS) -std=c++11

all: server\


clean:
	rm -f server
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

server: server.cpp getport.h getport.c md5sum.h md5sum.c tscounter.h tscounter.c timedaccept.c
	g++ $(COPTS) $(CFLAGS) md5sum.c server.cpp getport.c -o server


