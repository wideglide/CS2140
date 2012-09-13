CFLAGS  = -Iinclude -Wall 
CC = gcc
LDFLAGS = -Llib -lcomb
VPATH  += src

LIBS = lib/libcomb.a lib/libcomb.so
BINS = combtest combtest-0 combtest-1 combtest-2 
OBJS = lucas.o bell.o clist.o


.PHONY: all clean install uninstall

all: $(BINS)

clean:
	rm -f *.o *.s *.i $(LIBS) $(BINS)


#
# Rules for BINS
#
combtest:combtest.c $(OBJS)
	$(CC) $^ -o $@
	
combtest-0: combtest.c lib/libcomb.a
	$(CC) $^ -o $@

combtest-1: combtest.o lib/libcomb.so
	$(CC) $< $(LDFLAGS) -o $@

combtest-2: combtest.c $(OBJS)
	$(CC) $(CFLAGS) --static $^ -o $@

#
# Rules for LIBS
#
lib/libcomb.a: $(OBJS)
	ar crv $@ $(OBJS)
	
lib/libcomb.so: $(OBJS)
	$(CC) -shared $(OBJS) -o $@


#
# Rules for Object files, pre-processor output, assembly
#

%.i:%.c
	$(CC) $(CFLAGS) -E $< -o $@

%.s:%.c
	$(CC) $(CFLAGS) -S $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@


#
# install and uninstall targets
#

install: /usr/lib/libcomb.so

/usr/lib/libcomb.so: lib/libcomb.so
	sudo install $< $@ -m 0444

uninstall:
	sudo rm -f /usr/lib/libcomb.so

#
# create dist file for submission
#

dist:dist.tgz

dist.tgz: Makefile lab5-report.pdf combtest.c lucas.c bell.c clist.c
	tar -cvzf dist.tgz Makefile combtest.c src/lucas.c src/bell.c \
	src/clist.c lab5-report.pdf report.txt

#
# Dependencies
#
combtest.o: combtest.c include/bell.h include/combinatorics.h \
                       include/lucas.h include/clist.h
lucas.o: lucas.c include/lucas.h
bell.o: bell.c include/bell.h
clist.o: clist.c include/clist.h include/combinatorics.h

