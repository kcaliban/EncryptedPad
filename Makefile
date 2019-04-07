# Compiler
CC=gcc
# Libraries
LIBS=-lgcrypt -lncurses
# Dependencies
DEPS=primitive_txt.c crypt.c io.c 

# Will always be executed
.PHONY: clean all

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(LIBS)

all: crypt.o io.o primitive_txt.o
	$(CC) crypt.o io.o primitive_txt.o main.c -o encrn $(LIBS)

clean:
	rm -rf *.o
