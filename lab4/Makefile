# This makefile compiles the program my_sort or op_sort
#
# Author  : Bundt, Josh
# Class   : CS2140 
# Project : Lab 4
# Date    : 03 August 2011

# Configuration:
#
CC = cc
CFLAGS = -Wall -g
OFLAGS = -Wall -O3
DIST = tar -cvzf dist.tgz r_calc.c lab-answers.txt Makefile

# build targets
all: my_sort

my_sort: my_sort.c
	$(CC) $(CFLAGS) $< -o $@

op_sort: my_sort.c
	$(CC) $(OFLAGS) $< -o $@

# do some housekeeping
clean:
	rm -f *.o my_sort op_sort

distclean: clean
	rm -f dist*.tgz

# archive the project files
dist: dist.tgz

dist.tgz: my_sort.c Makefile test.sh write_up.pdf
	tar -cvzf dist.tgz my_sort.c Makefile test.sh write_up.pdf
