# MAKEFILE for the echo server

CC = gcc
CFLAGS = -Wall -pthread

# default target
all: server

# compile the server
server: server.o
	$(CC) $(CFLAGS) server.o -o server  # This line must start with a tab

# compile the object file
server.o: server.c
	$(CC) $(CFLAGS) -c server.c  # This line must also start with a tab

# clean up generated files
clean:
	rm -f *.o server
