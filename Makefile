AR=ar
CC=gcc
RM=rm

LDFLAGS=-Lsrc/lib -lbtcapi -lbtcutil $(JANSSONLIBS) $(CURLLIBS)
CFLAGS=-Wall -Wextra -Wpedantic -std=gnu11 -march=native -O2 -DDEBUG=0 -D_GNU_SOURCE $(JANSSONFLAGS) $(CURLFLAGS)
CFLAGS_DEBUG=-Wall -Wextra -Wpedantic -std=gnu11 -march=native -Og -g -DCC=$(MCC) -DDEBUG=1 -D_GNU_SOURCE $(JANSSONFLAGS) $(CURLFLAGS)

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

btcwatch: src/main
	cp src/main ./btcwatch

btcwatch-debug: src/main_debug
	cp src/main ./btcwatch-debug

src/main: src/main.o src/lib/libbtcapi.a src/lib/libbtcutil.a

src/main_debug: src/main_debug.o src/lib/libbtcapi_debug.a src/lib/libbtcutil_debug.a

lib%.a: %.o
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

lib%_debug.a: %_debug.o
	$(AR) rcs $@ $^

%_debug.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS_DEBUG) -c -o $@ $^

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
