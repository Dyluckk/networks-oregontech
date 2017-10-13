# simple Makefile for creating the unit test executable

COPTS = -g -O0 -Wall

all: lab2.spec

clean:
	rm -f lab2.spec
	rm -f *.o

.c.o:
	g++ $(COPTS) -c $? -o $@

lab2.spec: ./lib/unity/unity.h ./lib/unity/unity.c lab2.spec.c $(OBJS)
	g++ $(COPTS) ./lib/unity/unity.c lab2.spec.c -o lab2.spec $(OBJS)
