/*
	Copyright (C) 2013-2014 Marco Scannadinari <m@scannadinari.co.uk>

	This file is part of btcwatch.

	btcwatch is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	btcwatch is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with btcwatch.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BTCUTIL_H
#define BTCUTIL_H

#define noreturn _Noreturn
#define bputs(str) fputs(str, stdout)

#include <stdbool.h>
#include <stdint.h>

#include "../lib/libbtcapi/btcapi.h"

// finds absolute path to ~/.btcwatch and ~/.btcwatch/btcstore
void find_path(char *const path, char *const pathwf);

// prints usage info and options
noreturn void help(const char *const prog_nm, const char *const topic);

// output rates with additional information if needed
void print_rates(
	btc_rates_t *rates,
	btc_err_t *err,
	uint_fast8_t to_print,
	uint_fast32_t n,
	bool verbose,
	bool reverse,
	bool colour
);

// prints version number
noreturn void version(void);

#endif
