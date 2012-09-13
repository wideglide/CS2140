# Bundt, Josh		CS2140 - Lab 6
#
# Makefile for fsinfo project
#

CFLAGS = -Wall -g
CC = gcc

.PHONY: all clean

all: fsinfo

fsinfo: fsinfo.c
	@echo Creating $@ from $<
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o fsinfo 
	
dist:dist.tgz

dist.tgz:Makefile fsinfo.c report.txt 
	tar -cvzf dist.tgz Makefile fsinfo.c report.txt
