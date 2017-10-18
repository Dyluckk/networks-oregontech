#**************************************************
# Makefile for OIT Nameserver lab
# CST 415 Lab 2
#
# Author: Zachary Wentworth
# Email:  zachary.wentworth@oit.edu
#
COPTS = -g -O0 -Wall
CFLAGS := $(CFLAGS) -std=c++11

all: oit_ns\
	 oit_ns.spec

clean:
	rm -f oit_ns
	rm -f oit_ns.spec
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

oit_ns: oit_ns.cpp nameserver.h ns_handler.h ns_handler.cpp encode.h encode.cpp
	g++ $(COPTS) $(CFLAGS) oit_ns.cpp -o oit_ns ./ns_handler.cpp encode.cpp

oit_ns.spec: ./lib/unity/unity.h ./lib/unity/unity.c oit_ns.spec.cpp $(OBJS)
	g++ $(COPTS) ./lib/unity/unity.c oit_ns.spec.cpp -o oit_ns.spec $(OBJS)
