CC = cc
CFLAGS = -Wall -g

all: hellow etime

hellow: hellow.c
etime: etime.c

etime hellow:
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: goodbye
goodbye: hellow
	./hellow >> goodbye
	@echo "Farewell, World" >> goodbye

dist: hellow.c etime.c Makefile
	tar -cvzf dist.tgz hellow.c etime.c Makefile

backup: hellow.c etime.c Makefile
	-mv -f dist.tgz dist.`date +%Y%m%d%s`.tgz
	tar -cvzf dist.tgz hellow.c etime.c Makefile

clean:
	rm -f *.o hellow etime goodbye

distclean: clean
	rm -f dist*.tgz
