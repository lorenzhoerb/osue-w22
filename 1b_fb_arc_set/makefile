# @file Makefile
# @author Lorenz Hörburger (12024737) 
# @date 10.11.2022
#
# @brief Makefile
#
# Program names: generator, supervisor
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
CFLAGS = -std=c99 -pedantic -Wall $(DEFS)
LFLAGS = -pthread -lrt

OBJECTS = generator.o supervisor.o 

all: generator supervisor

generator: generator.o graph.o log.o
	$(CC) -o $@ $^ $(LFLAGS)

supervisor: supervisor.o log.o
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


supervisor.o: supervisor.c cbuffer.h common.h log.h
generator.o: generator.c cbuffer.h common.h graph.h log.h
graph.o: graph.c graph.h
log.o: log.c log.h

clean: 
	rm -rf *.o generator supervisor