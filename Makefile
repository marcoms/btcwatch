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

# all Makefile-generated files with .o extension
allo=$(addsuffix .o, $(all))

# all Makefile-generated files with .a extension and "lib" prefix
alla=$(addsuffix .a, $(libwpre))

curlflags=$(shell pkg-config libcurl --cflags --libs)
janssonflags=$(shell pkg-config jansson --cflags --libs)

all:
	$(CC) -o$(libdir)/btcapi.o $(libdir)/btcapi.c -c $(CFLAGS)
	ar rc $(libdir)/libbtcapi.a $(libdir)/btcapi.o
	ranlib $(libdir)/libbtcapi.a
	@echo
	
	$(CC) -o$(libdir)/cmdlineutils.o $(libdir)/cmdlineutils.c -c $(CFLAGS)
	ar rc $(libdir)/libcmdlineutils.a $(libdir)/cmdlineutils.o
	ranlib $(libdir)/libcmdlineutils.a
	@echo
	
	$(CC) -o $(srcdir)/main.o $(srcdir)/main.c -c $(CFLAGS)
	@echo
	
	$(CC) -o btcwatch $(srcdir)/main.o -Lsrc/lib -lbtcapi -lcmdlineutils $(curlflags) $(janssonflags)

clean:
	$(foreach i, $(all), rm -rf $(i)${\n})
	@echo
	
	$(foreach i, $(alla), rm -rf $(i)${\n})
	@echo
	
	$(foreach i, $(allo), rm -rf $(i)${\n})
