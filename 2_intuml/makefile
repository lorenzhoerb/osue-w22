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
OBJECTS = intuml.o hexlib.o

all: intuml

intuml: $(OBJECTS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -rf *.o intuml
