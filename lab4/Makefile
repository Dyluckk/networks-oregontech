#**************************************************
# Makefile for TCP file transfer server
# CST 415 Lab 4
#
#
COPTS = -g -O0 -Wall -pthread
CFLAGS := $(CFLAGS) -std=c++11

all: server\
	 clearport\

clean:
	rm -f server
	rm -f clearport
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

server: server.cpp getport.h getport.c md5sum.h md5sum.c timedaccept.c
	g++ $(COPTS) $(CFLAGS) md5sum.c server.cpp getport.c -o server

clearport: clearport.c getport.h getport.c
	g++ $(COPTS) $(CFLAGS) clearport.c getport.c -o clearport
