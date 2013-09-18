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

#define URL_API "https://data.mtgox.com/api/2/BTCUSD/money/ticker_fast"

#include "../config.h"

#if HAVE_LIBC
	#include <stdio.h>				// printf
	#include <getopt.h>				// getopt
#else
	#error libc not found
#endif

#include "include/btcapi.h"			// rates_t, get_api, parse_json
#include "include/cmdlineutils.h"	// help
#include "include/errutils.h"		// ERR

int main(int argc, char** argv) {
	char *api;
	int opt;
	char optstring[] = "?hv";
	rates_t rates;

	while((opt = getopt(argc, argv, optstring)) != -1) {
		switch(opt) {
			case '?': case 'h':
				help(argv[0], optstring);
				break;

			case 'v':
				version(argv[0], "0.0.1");
				break;

			default:
				fprintf(
					stderr,
					"%s: error: no such option: %c\n",
					argv[0],
					opt
				);
		}
	}

	api = get_api(URL_API, argv[0]);
	rates = parse_json(api, argv[0]);

	printf(
		"result: %s\nbuy: %f\nsell: %f\n",
		rates.result ? "success" : "failure",
		rates.buy,
		rates.sell
	);

	return 0;
}
