CC=gcc
CFLAGS=-W -Wall -Wextra
LIBFLAGS=-lmylib
INC=-Iinclude/

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: bin/test_1 bin/test_2

bin/test_1: bin/libmylib.a tests/test_1.c
	$(CC) $(CFLAGS) $(INC) -o $@ tests/test_1.c bin/libmylib.a

bin/test_2: bin/libmylib.a tests/test_2.c
	$(CC) $(CFLAGS) $(INC) -o $@ tests/test_2.c bin/libmylib.a

bin/libmylib.a: $(OBJ)
	ar rcs $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c -MMD -MP -o $@ $<

-include $(DEP)

clean:
	rm -rf $(OBJ) $(DEP) bin/*

