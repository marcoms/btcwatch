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
	#include <stdlib.h>		// exit(), EXIT_SUCCESS
	#include <stdio.h>		// printf()
#else
	#error libc not found
#endif

#include "../include/cmdlineutils.h"

noreturn void help(const char *const prog_name, const char *const optstring) {
	printf(
		"usage: %s -%s\n"
		"get Bitcoin trade information\n"
		"\n"
		"options:\n"
		"  -?, -h         print this help\n"
		"  -V             print version number\n"
		"  -b             print buy price\n"
		"  -c currency    set conversion currency\n"
		"  -p             check for a successful JSON response\n"
		"  -s             print sell price\n"
		"  -v             increase verbosity\n"
		"\n"
		"report bugs to <marco@scannadinari.co.uk>\n",
		prog_name,
		optstring
	);

	exit(EXIT_SUCCESS);
}

noreturn void version(const char *const prog_name, const char *const version) {
	printf("%s %s\n", prog_name, version);

	exit(EXIT_SUCCESS);
}
