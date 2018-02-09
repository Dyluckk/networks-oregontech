#**************************************************
# Makefile for lookup
#**************************************************
COPTS = -g -O0 -Wall -pthread
CFLAGS := $(CFLAGS) -std=c++11

all: lookup\

clean:
	rm -f lookup
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

lookup: lookup.cpp timedread.h timedread.cpp
	g++ $(COPTS) $(CFLAGS) lookup.cpp timedread.cpp -o lookup
