# simple Makefile for creating the unit test executable

COPTS = -g -O0 -Wall
CFLAGS := $(CFLAGS) -std=c++11

all: ns_handler.spec

clean:
	rm -f ns_handler.spec
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

ns_handler.spec: ../lib/unity/unity.h ../lib/unity/unity.c ns_handler.spec.c ../src/ns_handler.h ../src/ns_handler.cpp $(OBJS)
	g++ $(COPTS) $(CFLAGS) ../lib/unity/unity.c ns_handler.spec.c -o ns_handler.spec ../src/ns_handler.cpp $(OBJS)
