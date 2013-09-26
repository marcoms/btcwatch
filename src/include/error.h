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

#ifndef ERRUTILS_H
#define ERRUTILS_H

#include <stdarg.h>

static inline int error(const char *const prog_name, const char *const format, ...) {
	va_list args;
	int nchars;

	va_start(args, format);

	nchars = printf("%s: error: ", prog_name);
	nchars += vfprintf(stderr, format, args);

	putchar('\n');

	va_end(args);

	return nchars;
}

#endif
