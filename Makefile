INSTALL_PREFIX=/home/jmft2/local
CFLAGS=-std=c11

all: libpnghelpers.a

libpnghelpers.a: clean pnghelpers.o
	ar -cvq libpnghelpers.a pnghelpers.o

install: libpnghelpers.a
	cp -i pnghelpers.h $(INSTALL_PREFIX)/include/
	cp -i libpnghelpers.a $(INSTALL_PREFIX)/lib/

uninstall:
	rm -i $(INSTALL_PREFIX)/include/pnghelpers.h
	rm -i $(INSTALL_PREFIX)/lib/libpnghelpers.a

clean:
	rm -f pnghelpers.o libpnghelpers.a

