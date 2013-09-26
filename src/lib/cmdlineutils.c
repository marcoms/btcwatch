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

#include <stdlib.h>						// exit(), EXIT_SUCCESS
#include <stdio.h>						// printf()

#include "../include/cmdlineutils.h"	// help(), version()
#include "../include/debug.h"			// DBG()

noreturn void help(const char *const prog_name, const char *const optstring) {
	#if DEBUG
	debug("help()");
	#endif

	printf(
		"usage: %s -[%s]\n"
		"get Bitcoin trade information\n"
		"\n"
		"options:\n"
		"  -?, -h         --help                 print this help\n"
		"  -V             --version              print version number\n"
		"  -b             --buy                  print buy price\n"
		"  -c CURRENCY    --currency=CURRENCY    set conversion currency\n"
		"  -p             --ping                 check for a successful JSON response\n"
		"  -s             --sell                 print sell price\n"
		"  -v             --verbose              increase verbosity\n"
		"\n"
		"report bugs to <marco@scannadinari.co.uk>\n",
		prog_name,
		optstring
	);

	exit(EXIT_SUCCESS);
}

noreturn void version(const char *const prog_name, const char *const version) {
	#if DEBUG
	debug("version()");
	#endif

	printf("%s %s\n", prog_name, version);

	#if DEBUG
	printf("compiled with %s %s (excluding macros)\n", CC, CFLAGS);
	#endif

	exit(EXIT_SUCCESS);
}
