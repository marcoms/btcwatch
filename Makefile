define \n


endef

BOLD="\033[1m"
RESET="\033[0m"
INDENT=@echo -n "  "
NEWL=@echo

BTCWATCH_VERSION=0.0.1

ifdef $(CC)
	MCC=$(CC)
else
	MCC=gcc
endif

MCFLAGS=-Wall -Wextra -Wpedantic -std=gnu11 -march=native -O2 -DBTCWATCH_VERSION="\"$(BTCWATCH_VERSION)\""
ifdef $(CFLAGS)
	MCFLAGS+=$(CFLAGS)
endif

MCFLAGS_DEBUG=-Wall -Wextra -Wpedantic -std=gnu11 -march=native -Og -g -DBTCWATCH_VERSION="\"$(BTCWATCH_VERSION)\""

PREFIX=$(shell cat prefix.txt)

# contents of src/ dir
SRC=src/main

# contents of lib/ dir
LIB=$(addprefix	src/lib/, btcapi cmdlineutils)

# all Makefile-generated files without {suf,pre}fixes
ALL=$(SRC) $(LIB)

ALLC=$(addsuffix .c, $(ALL))

# all Makefile-generated files with .o extension
ALLO=$(addsuffix .o, $(ALL))

# all Makefile-generated files with .a extension and "lib" prefix
ALLA=$(addprefix src/lib/, $(addsuffix .a, libbtcapi libcmdlineutils))

CURLFLAGS=$(shell pkg-config libcurl --cflags --libs)
JANSSONFLAGS=$(shell pkg-config jansson --cflags --libs)

all: src/main.o src/lib/libbtcapi.a src/lib/libcmdlineutils.a
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -obtcwatch $< -Lsrc/lib/ -lbtcapi -lcmdlineutils $(CURLFLAGS) $(JANSSONFLAGS)
	$(NEWL)

debug: src/main_debug.o src/lib/libbtcapi.a src/lib/libcmdlineutils.a
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -obtcwatch-debug $< -Lsrc/lib/ -lbtcapi -lcmdlineutils $(CURLFLAGS) $(JANSSONFLAGS)
	$(NEWL)

src/main.o: src/main.c
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -o$@ $^ -c $(MCFLAGS)
	$(NEWL)

src/main_debug.o: src/main.c
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -o$@ $^ -c $(MCFLAGS_DEBUG) -DDEBUG -DCC="\"$(MCC)\"" -DCFLAGS="\"$(MCFLAGS_DEBUG)\""
	$(NEWL)

src/lib/btcapi.o: src/lib/btcapi.c
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -o$@ $^ -c $(MCFLAGS)
	$(NEWL)

src/lib/libbtcapi.a: src/lib/btcapi.o
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	ar rc $@ $^
	$(INDENT)
	ranlib $@
	$(NEWL)

src/lib/cmdlineutils.o: src/lib/cmdlineutils.c
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -o$@ $^ -c $(MCFLAGS)
	$(NEWL)

src/lib/libcmdlineutils.a: src/lib/cmdlineutils.o
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	ar rc $@ $^
	$(INDENT)
	ranlib $@
	$(NEWL)

install: all
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	@mkdir $(PREFIX)/bin/
	$(INDENT)
	install -m777 btcwatch $(PREFIX)/bin/
	$(NEWL)

install-strip: all
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	@mkdir $(PREFIX)/bin/
	$(INDENT)
	install -s -m777 btcwatch $(PREFIX)/bin/
	$(NEWL)

uninstall:
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	rm -rf $(PREFIX)/bin/btcwatch
	$(NEWL)

clean:
	@echo -e $(BOLD)$@$(RESET)
	$(foreach i, $(ALL), $(INDENT) ${\n} rm -rf $(i) ${\n})  @# this works somehow
	$(foreach i, $(ALLA), $(INDENT) ${\n} rm -rf $(i) ${\n})
	$(foreach i, $(ALLO), $(INDENT) ${\n} rm -rf $(i) ${\n})
	$(INDENT)
	rm -rf btcwatch
	$(NEWL)

distclean: clean
	@echo -e $(BOLD)$@$(RESET)
	$(foreach i, $(wildcard config.*), $(INDENT) ${\n} rm -rf $(i) ${\n})
	$(INDENT)
	rm -rf autom4te.cache
	$(INDENT)
	rm -rf prefix.txt
	$(INDENT)
	rm -rf btcwatch
	$(NEWL)

