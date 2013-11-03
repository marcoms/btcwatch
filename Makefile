define \n


endef

define NEWL
@echo
endef

define RESET
"\033[0m"
endef

AR=ar
CC=gcc
RM=rm

LDFLAGS=-Lsrc/lib -lbtcapi -lbtcutil $(JANSSONLIBS) $(CURLLIBS)

CFLAGS=-Wall -Wextra -Wpedantic -std=gnu11 -march=native -O2 -DDEBUG=0 -D_GNU_SOURCE $(JANSSONFLAGS) $(CURLFLAGS)
MCFLAGS_DEBUG=-Wall -Wextra -Wpedantic -std=gnu11 -march=native -Og -g -DCC="\"$(MCC)\"" -DDEBUG=1 -D_GNU_SOURCE

PREFIX=$(shell cat prefix.txt)

# contents of src/ dir
SRC=$(addprefix src/, main main_debug)

# contents of lib/ dir
LIB=$(addprefix src/lib/, btcapi btcapi_debug btcutil btcutil_debug)

# all Makefile-generated files without {suf,pre}fixes
ALL=$(SRC) $(LIB)

ALLC=$(addsuffix .c, $(ALL))

# all Makefile-generated files with .o extension
ALLO=$(addsuffix .o, $(ALL))

# all Makefile-generated files with .a extension and "lib" prefix
ALLA=$(addprefix src/lib/, $(addsuffix .a, $(addprefix lib, btcapi btcapi_debug btcutil btcutil_debug)))

CURLLIBS=$(shell pkg-config libcurl --libs)
JANSSONLIBS=$(shell pkg-config jansson --libs)
CURLFLAGS=$(shell pkg-config libcurl --cflags)
JANSSONFLAGS=$(shell pkg-config jansson --cflags)

all: btcwatch

# basicallly make as much as you can implicit for some reason

btcwatch: src/main.o src/lib/libbtcapi.a src/lib/libbtcutil.a

debug: src/main_debug.o src/lib/libbtcapi_debug.a src/lib/libbtcutil_debug.a
	$(MCC) -obtcwatch-debug $< -Lsrc/lib -lbtcapi_debug -lbtcutil_debug $(CURLLIBS) $(CURLFLAGS) $(JANSSONLIBS) $(JANSSONFLAGS)

src/main: src/main.o src/lib/libbtcapi.a src/lib/libbtcutil.a

src/main_debug.o: src/main.c
	${TITLE}
	$(MCC) -o$@ $< -c $(MCFLAGS_DEBUG)
	${NEWL}

src/lib/btcapi.o: src/lib/btcapi.c

src/lib/libbtcapi.a: src/lib/btcapi.o
	${TITLE}
	ar rc $@ $<
	ranlib $@
	${NEWL}

src/lib/btcapi_debug.o: src/lib/btcapi.c
	${TITLE}
	$(MCC) -o$@ $< -c $(MCFLAGS_DEBUG)
	${NEWL}

src/lib/libbtcapi_debug.a: src/lib/btcapi_debug.o
	${TITLE}
	ar rc $@ $<
	ranlib $@
	${NEWL}

src/lib/btcutil.o: src/lib/btcutil.c

src/lib/libbtcutil.a: src/lib/btcutil.o
	${TITLE}
	ar rc $@ $<
	ranlib $@
	${NEWL}

src/lib/btcutil_debug.o: src/lib/btcutil.c
	${TITLE}
	$(MCC) -o $@ $< -c $(MCFLAGS_DEBUG)
	${NEWL}

src/lib/libbtcutil_debug.a: src/lib/btcutil_debug.o
	${TITLE}
	ar rc $@ $<
	ranlib $@
	${NEWL}

install: all
	${TITLE}
	mkdir -p $(PREFIX)/bin/
	install -m777 btcwatch* $(PREFIX)/bin/
	${NEWL}

install-strip: install
	${TITLE}
	strip -s $(PREFIX)/bin/btcwatch
	${NEWL}

uninstall:
	${TITLE}
	rm -rf $(PREFIX)/bin/btcwatch
	$(INDENT)
	rm -rf $(PREFIX)/bin/btcwatch-debug
	${NEWL}

clean:
	${TITLE}
	$(foreach i, $(ALL), ${\n} rm -rf $(i) ${\n})
	$(foreach i, $(ALLA), ${\n} rm -rf $(i) ${\n})
	$(foreach i, $(ALLO), ${\n} rm -rf $(i) ${\n})
	rm -rf btcwatch
	rm -rf btcwatch-debug
	${NEWL}

distclean: clean
	${TITLE}
	$(foreach i, $(wildcard config.*), $(INDENT) ${\n} rm -rf $(i) ${\n})
	rm -rf autom4te.cache
	rm -rf src/include/config.h
	rm -rf prefix.txt
	${NEWL}

