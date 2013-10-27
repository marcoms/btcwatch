/*
	Copyright (C) 2013 Marco Scannadinari.

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

#define BOLD "\033[1m"
#define RESET "\033[0m"

#define IGNORE(x) (void) (x)  // removes unused parameter/variable warnings

#include <stdarg.h>
#include <stdio.h>

#include "../include/btcdbg.h"

void btcdbg(const char *const fmt, ...) {
	#if (DEBUG)
	va_list args;

	va_start(args, fmt);

	fputs(BOLD "DEBUG: ", stdout);
	vprintf(fmt, args);
	puts(RESET);

	va_end(args);
	#else
	IGNORE(fmt);
	#endif
}
