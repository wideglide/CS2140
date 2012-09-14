# Bundt, Josh			CS2140			Lab 9 - wserver 
#
# Makefile for wserver
#

CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lpthread
BINS=wserver

.PHONY: all clean dist

all: $(BINS)

wserver: wserver.o http_support.o

$(BINS):
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -f *.o $(BINS)
	
dist: dist.tgz

dist.tgz: Makefile wserver.c http_support.h http_support.c
	tar -cvzf dist.tgz Makefile wserver.c http_support.h http_support.c

