# @file Makefile 
# @author Lorenz Hörburger (12024737)  
# @date 10.11.2022 
# 
# @brief Makefile 
# 
# Program name: ispalindrom
CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
CFLAGS = -std=c99 -pedantic -Wall $(DEFS)

all: ispalindrom

ispalindrom: ispalindrom.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o ispalindrom