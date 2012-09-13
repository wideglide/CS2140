# This makefile compiles the program r_calc
#
# Author  : Bundt, Josh
# Class   : CS2140 
# Project : Lab 2
# Date    : 18 July 2011

# Configuration:
#
CC = cc
CFLAGS = -Wall -g
DIST = tar -cvzf dist.tgz r_calc.c lab-answers.txt Makefile

# build targets
all: r_calc

r_calc: r_calc.c
	$(CC) $(CFLAGS) $< -o $@

# do some housekeeping
clean:
	rm -f *.o r_calc

distclean: clean
	rm -f dist*.tgz

# archive the project files
dist: dist.tgz

backup: r_calc.c lab-answers.txt Makefile
	-mv -f dist.tgz dist.`date +%Y%m%d%s`.tgz
	make dist

dist.tgz: r_calc.c lab-answers.txt Makefile
	tar -czvf $@$^
