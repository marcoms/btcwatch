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

#if HAVE_LIBC
	#include <stdlib.h>		// exit
	#include <stdio.h>		// printf
#else
	#error libc not found
#endif

#include "../include/cmdlineutils.h"

noreturn void help(const char *const prog_name, const char *const optstring) {
	printf("usage: %s -%s\n", prog_name, optstring);
	printf("get Bitcoin trade information from MtGox\n\n");
	printf("    -?, -h [topic]  --help[=topic]       print this help\n");
	printf("    -b              --buy                print buy price\n");
	//printf("    -c currency     --currency=currency  set conversion currency\n");
	printf("    -p              --ping               check for a successful JSON response\n");
	printf("    -s              --sell               print sell price\n");
	printf("    -v              --version            print version number\n");
	printf("\n");
//	printf("see '%s --help topics' for a list of topics\n", prog_name);
	printf("report bugs to <marco@scannadinari.co.uk>\n");

	exit(EXIT_SUCCESS);
}

noreturn void version(const char *const prog_name, const char *const version) {
	printf("%s %s\n", prog_name, version);

	exit(EXIT_SUCCESS);
}
