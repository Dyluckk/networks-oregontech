# Using µnit is very simple; just include the header and add the C
# file to your sources.  That said, here is a simple Makefile to build
# the lab1.spec.

CSTD:=99
OPENMP:=n
ASAN:=n
UBSAN:=n
EXTENSION:=
TEST_ENV:=
CFLAGS:=
AGGRESSIVE_WARNINGS=n

ifeq ($(CC),pgcc)
        CFLAGS+=-c$(CSTD)
else
        CFLAGS+=-std=c$(CSTD)
endif

ifeq ($(OPENMP),y)
        ifeq ($(CC),pgcc)
                CFLAGS+=-mp
        else
                CFLAGS+=-fopenmp
        endif
endif

ifneq ($(SANITIZER),)
        CFLAGS+=-fsanitize=$(SANITIZER)
endif

ifneq ($(CC),pgcc)
        ifeq ($(EXTRA_WARNINGS),y)
                CFLAGS+=-Wall -g -Wextra -Werror
        endif

        ifeq ($(ASAN),y)
                CFLAGS+=-fsanitize=address
        endif

        ifeq ($(UBSAN),y)
                CFLAGS+=-fsanitize=undefined
        endif
endif

lab1.spec$(EXTENSION): ./lib/unit/munit.h ./lib/unit/munit.c ./src/readline.h ./src/readline.c ./src/readblocks.h ./src/readblocks.c ./src/nameserver.h ./src/encode.h ./src/encode.c  lab1.spec.c
	$(CC) $(CFLAGS) -o $@ -g ./src/readline.c ./src/readblocks.c ./src/encode.c ./lib/unit/munit.c lab1.spec.c

test:
	$(TEST_ENV) ./lab1.spec$(EXTENSION)

clean:
	rm -f lab1.spec$(EXTENSION)

all: lab1.spec$(EXTENSION)
