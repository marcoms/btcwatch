/*
	Copyright (C) 2013 Marco Scannadinari

	This file is part of btcwatch.

	btcwatch is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	btcwatch is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with btcwatch.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../../config.h"

#include <stdlib.h>
#include <stdio.h>

#include "../include/cmdlineutils.h"
#include "../include/debug.h"

noreturn void help(const char *const prog_name, const char *const optstring) {
	#if DEBUG
	debug(__FILE__, __LINE__, "help()");
	#endif

	printf("usage: %s -[%s]\n", prog_name, optstring);
	puts(
		"get Bitcoin trade information\n"
		"\n"
		"options:\n"
		"  -?, -h         --help                 print this help\n"
		"  -V             --version              print version number\n"
		"  -a             --all                  equivalent to -pbs\n"
		"  -b             --buy                  print buy price\n"
		"  -c CURRENCY    --currency=CURRENCY    set conversion currency\n"
		"  -p             --ping                 check for a successful JSON response\n"
		"  -s             --sell                 print sell price\n"
		"  -v             --verbose              increase verbosity\n"
		"\n"
		"report bugs to <marco@scannadinari.co.uk>"
	);

	exit(EXIT_SUCCESS);
}

noreturn void version(const char *const prog_name, const char *const version) {
	#if DEBUG
	debug(__FILE__, __LINE__, "version()");
	#endif

	printf("%s %s\n", prog_name, version);

	#if DEBUG
	debug(__FILE__, __LINE__, "compiled with %s %s (excluding macros)\n", CC, CFLAGS);
	#endif

	exit(EXIT_SUCCESS);
}
