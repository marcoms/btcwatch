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

#include <stdarg.h>
#include <stdio.h>

#include "../include/btcerr.h"

void btcerr(const char *const prog_name, const char *const format, ...) {
	#if DEBUG
	debug(__FILE__, __LINE__, "error()");
	#endif

	va_list args;

	va_start(args, format);

	fprintf(stderr, "%s: error: ", prog_name);
	vfprintf(stderr, format, args);

	putchar('\n');

	va_end(args);
}
