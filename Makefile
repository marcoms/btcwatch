define \n


endef

CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=gnu99 -march=native -O3

srcdir=src
libdir=src/lib
includedir=src/include

# contents of src/ dir
src=$(srcdir)/main

# contents of lib/ dir
lib=$(addprefix $(libdir)/, btcapi cmdlineutils)

# contents of lib/ dir with "lib" prefix
libwpre=$(addprefix $(libdir)/, $(addprefix lib, btcapi cmdlineutils))

# all Makefile-generated files without {suf,pre}fixes
all=$(src) $(lib)

allc=$(addsuffix .c, $(all))

# all Makefile-generated files with .o extension
allo=$(addsuffix .o, $(all))

# all Makefile-generated files with .a extension and "lib" prefix
alla=$(addsuffix .a, $(libwpre))

curlflags=$(shell pkg-config libcurl --cflags --libs)
janssonflags=$(shell pkg-config jansson --cflags --libs)

all: src/main.o src/lib/libbtcapi.a src/lib/libcmdlineutils.a
	$(CC) -obtcwatch -L$(libdir)/ -lbtcapi -lcmdlineutils $(curlflags) $(janssonflags) $<

src/main.o: src/main.c
	$(CC) -c $(CFLAGS) -o$@ $^

src/lib/btcapi.o: src/lib/btcapi.c
	$(CC) -c $(CFLAGS) -o$@ $^

src/lib/libbtcapi.a: src/lib/btcapi.o
	ar rcu $@ $^
	ranlib $@

src/lib/cmdlineutils.o: src/lib/cmdlineutils.c
	$(CC) -c $(CFLAGS) -o$@ $^

src/lib/libcmdlineutils.a: src/lib/cmdlineutils.o
	ar rcu $@ $^
	ranlib $@

clean:
	$(foreach i, $(all), rm -rf $(i)${\n})
	@echo
	
	$(foreach i, $(alla), rm -rf $(i)${\n})
	@echo
	
	$(foreach i, $(allo), rm -rf $(i)${\n})
