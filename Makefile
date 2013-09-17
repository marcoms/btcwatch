define \n


endef

BOLD="\033[1m"
RESET="\033[0m"
INDENT=@echo -n "  "
ENDL=@echo

MCC=$(CC)
MCFLAGS=-Wall -Wextra -Wpedantic -std=gnu99 -march=native -O3
MCFLAGS+=$(CFLAGS)

# contents of src/ dir
src=src/main

# contents of lib/ dir
lib=$(addprefix	src/lib/, btcapi cmdlineutils)

# all Makefile-generated files without {suf,pre}fixes
all=$(src) $(lib)

allc=$(addsuffix .c, $(all))

# all Makefile-generated files with .o extension
allo=$(addsuffix .o, $(all))

# all Makefile-generated files with .a extension and "lib" prefix
alla=$(addprefix src/lib/, $(addsuffix .a, libbtcapi libcmdlineutils))

curlflags=$(shell pkg-config libcurl --cflags --libs)
janssonflags=$(shell pkg-config jansson --cflags --libs)

all: src/main.o src/lib/libbtcapi.a src/lib/libcmdlineutils.a
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -obtcwatch $< -Lsrc/lib/ -lbtcapi -lcmdlineutils $(curlflags) $(janssonflags)
	$(ENDL)

src/main.o: src/main.c
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -o$@ $^ -c $(MCFLAGS)
	$(ENDL)

src/lib/btcapi.o: src/lib/btcapi.c
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -o$@ $^ -c $(MCFLAGS)
	$(ENDL)

src/lib/libbtcapi.a: src/lib/btcapi.o
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	ar rc $@ $^
	$(INDENT)
	ranlib $@
	$(ENDL)

src/lib/cmdlineutils.o: src/lib/cmdlineutils.c
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	$(MCC) -o$@ $^ -c $(MCFLAGS)
	$(ENDL)

src/lib/libcmdlineutils.a: src/lib/cmdlineutils.o
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	ar rc $@ $^
	$(INDENT)
	ranlib $@
	$(ENDL)

install: all
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	install -s -m777 btcwatch /usr/bin/
	$(ENDL)

uninstall:
	@echo -e $(BOLD)$@$(RESET)
	$(INDENT)
	rm /usr/bin/btcwatch
	$(ENDL)

clean:
	@echo -e $(BOLD)$@$(RESET)
	$(foreach i, $(all), $(INDENT) ${\n} rm -rf $(i) ${\n})  @# this works somehow
	$(foreach i, $(alla), $(INDENT) ${\n} rm -rf $(i) ${\n})
	$(foreach i, $(allo), $(INDENT) ${\n} rm -rf $(i) ${\n})
	$(INDENT)
	rm -rf btcwatch
	$(ENDL)
