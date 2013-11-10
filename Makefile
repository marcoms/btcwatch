AR=ar
CC=gcc
RM=rm

PREFIX=$(shell cat prefix.txt)

OUTBIN= \
	btcwatch \
	src/main \
	src/main.o \
	src/lib/libbtcapi.a \
	src/lib/btcapi.o \
	src/lib/libbtcutil.a \
	src/lib/btcutil.o

OUTCONF= \
	prefix.txt \
	config.log \
	config.status \
	src/include/config.h

CURLLIBS=$(shell pkg-config libcurl --libs)
JANSSONLIBS=$(shell pkg-config jansson --libs)
CURLFLAGS=$(shell pkg-config libcurl --cflags)
JANSSONFLAGS=$(shell pkg-config jansson --cflags)

LDFLAGS=$(JANSSONLIBS) $(CURLLIBS)
MCFLAGS=-Wall -Wextra -Wpedantic -std=gnu11 -D_GNU_SOURCE $(JANSSONFLAGS) $(CURLFLAGS)
MCFLAGS+=$(CFLAGS)
CFLAGS:=$(MCFLAGS)

btcwatch: src/main
	cp $^ $@

src/main: src/main.o src/lib/btcapi.o src/lib/btcutil.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

# (un)installation

install:
	mkdir -p $(PREFIX)/bin/
	install -m777 btcwatch* $(PREFIX)/bin/

install-strip: install
	strip -s $(PREFIX)/bin/btcwatch

uninstall:
	rm -rf $(PREFIX)/bin/btcwatch
	rm -rf $(PREFIX)/bin/btcwatch-debug

# cleaning

clean:
	$(RM) -f $(OUTBIN)

distclean: clean
	$(RM) -rf $(OUTCONF)
