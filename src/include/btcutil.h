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

#ifndef CMDLINEUTILS_H
#define CMDLINEUTILS_H

#define noreturn _Noreturn

#include <stdbool.h>

typedef struct {
	bool err;
	char errstr[64];
} btcerr_t;

// prints debug information in the form "DEBUG: " fmt 

void btcdbg(const char *const fmt, ...);

// finds absolute path to ~/.btcwatch and ~/.btcwatch/btcstore

void find_path(char *const path, char *const pathwf);

// prints usage info and options

noreturn void help(const char *const prog_nm);

// sets stdout to be byte-oriented

void resetb(void);

// sets stdout to be wide-oriented

void resetw(void);

// a quicker puts

void qputs(const char *str);

// prints version number

noreturn void version(void);

#endif
