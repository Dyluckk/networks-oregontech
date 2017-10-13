#**************************************************
# Makefile for OIT Nameserver lab
# CST 415 Lab 1
#
# Author: Philip Howard
# Email:  phil.howard@oit.edu
# Date:   Sept 22, 2016
#
COPTS = -g -O0 -Wall
CFLAGS := $(CFLAGS) -std=c++11

# OBJS = addr2str.o

all: oit_ns

clean:
	rm -f nsclient
	rm -f oit_ns
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

oit_ns: oit_ns.cpp nameserver.h ns_handler.h ns_handler.cpp
	g++ $(COPTS) $(CFLAGS) oit_ns.cpp -o oit_ns ../src/ns_handler.cpp

# nsclient: nsclient.c nameserver.h $(OBJS)
# gcc $(COPTS) nsclient.c -o nsclient $(OBJS)
