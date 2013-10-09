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

#ifndef BTCERR_H
#define BTCERR_H

#include <stdbool.h>

// struct containing error information to be passed to functions in btcapi.h
typedef struct {
	bool err;
	char errstr[64 + 1];
} btcerr_t;

// prints the program name, ": error: ", and passes format and a variable amount of arguments to vfprintf
void btcerr(const char *const prog_name, const char *const format, ...);

#endif
