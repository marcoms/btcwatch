PREFIX:=$(shell cat prefix.txt)

OUTBIN:= \
	btcwatch \
	src/main \
	src/main.o \
	src/lib/libbtcapi/btcapi.o \
	src/btcutil.o

OUTCONF:= \
	autom4te.cache \
	prefix.txt \
	config.log \
	config.status \
	src/include/config.h

CURLLIBS:=$(shell pkg-config libcurl --libs)
JANSSONLIBS:=$(shell pkg-config jansson --libs)
CURLFLAGS:=$(shell pkg-config libcurl --cflags)
JANSSONFLAGS:=$(shell pkg-config jansson --cflags)

LDFLAGS:=$(JANSSONLIBS) $(CURLLIBS)
MCFLAGS:=-Wall -Wextra -Wpedantic -std=gnu11 -pipe -D_GNU_SOURCE $(JANSSONFLAGS) $(CURLFLAGS)
MCFLAGS+=$(CFLAGS)
CFLAGS:=$(MCFLAGS)

all: btcwatch

btcwatch: src/main
	cp $^ $@

src/main: src/main.o src/lib/libbtcapi/btcapi.o src/btcutil.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

# (un)installation

install:
	mkdir -p $(PREFIX)/bin
	install -m777 btcwatch $(PREFIX)/bin

install-strip: install
	strip -s $(PREFIX)/bin/btcwatch

uninstall:
	rm -rf $(PREFIX)/bin/btcwatch

# cleaning

clean:
	$(RM) -f $(OUTBIN)

distclean: clean
	$(RM) -rf $(OUTCONF)

