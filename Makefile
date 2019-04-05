# Compiler
CC=gcc
# Libraries
LIBS=-lgcrypt -lncurses
# Dependencies
DEPS=crypt.c io.c

# Will always be executed
.PHONY: clean all

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(LIBS)

all: crypt.o io.o
	$(CC) crypt.o io.o main.c -o main $(LIBS)

clean:
	rm -rf *.o
