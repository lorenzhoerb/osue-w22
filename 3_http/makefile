# @file Makefile
# @author Lorenz Hörburger (12024737) 
# @date 08.01.2023
#
# @brief Makefile
#
# Program names: server, client
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
CFLAGS = -std=c99 -pedantic -Wall $(DEFS)
OBJECTS = server.o client.o

all: server client

server: server.o common.o
	$(CC) -o $@ $^ $(LFLAGS)

client: client.o common.o httpc.o
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

client.o: client.c common.h
server.o: server.c common.h
common.o: common.h
httpc.o: common.h httpc.h

clean: 
	rm -rf *.o server client
