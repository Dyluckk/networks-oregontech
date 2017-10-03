##**************************************
## Makefile
##
## Build routine for lab 1
##
## Author: Zachary Wentworth
## zachary.wentworth@oit.edu
##
## Date: Sept. 27, 2017
##

COPTS=-Wall -g -c  -O0 -std=c99

all: encode
	# nameserver\
	# readblocks\
	# readline

clean:
	rm -f $(OBJS)
	rm -f encode.o
	rm -f encode
	# rm -f nameserver.o
	# rm -f nameserver
	# rm -f readblocks.o
	# rm -f readblocks
	# rm -f readline.o
	# rm -f readline

.c.o:
	gcc $(COPTS) $? -o $@

encode: encode.o
	gcc encode.o -o encode

# nameserver: nameserver.o
# 	gcc nameserver.o -o nameserver
#
# readblocks: readblocks.o
# 	gcc readblocks.o -o readblocks
#
# readline: readline.o
# 	gcc readline.o -o readline
