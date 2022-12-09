# @file Makefile
# @author Lorenz Hörburger (12024737) 
# @date 20.11.2022
#
# @brief Makefile
#
# Program names: generator, supervisor
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
CFLAGS = -std=c99 -pedantic -Wall $(DEFS)
OBJECTS = intmul.o hexlib.o mulutil.o

all: intmul

intmul: $(OBJECTS)
	$(CC) -o $@ $^ $(LFLAGS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

intuml.o: intmul.c hexlib.o multutil.h
hexlib.o: hexlib.c hexlib.h
umlutil.o: multutil.c multutil.h

clean: 
	rm -rf *.o intmul
