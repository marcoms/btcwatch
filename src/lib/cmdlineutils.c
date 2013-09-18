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

void help(char *prog_name, char *optstring) {
	printf("usage: %s -%s\n", prog_name, optstring);
	printf("get Bitcoin trade information from MtGox\n\n");
	printf("    -?, -h    --help    print this help\n\n");
	printf("report bugs to <marco@scannadinari.co.uk> or https://github.com/marcoms/btcwatch/issues\n");

	exit(EXIT_SUCCESS);
}

void version(char *prog_name, char *version) {
	printf("%s %s\n", prog_name, version);

	exit(EXIT_SUCCESS);
}
