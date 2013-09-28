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

/*
Welcome to btcwatch's source!

Code is indented with tabs. Tabs were chosen for their reduced impact on file
sizes, and are much easier to edit and delete.

All comments should be followed with a space or new line, depending if they are
one-line or multi-line comments, respectively.

One-line comments are C99/C++-style //s. They should not be capitalised. If
they describe what a particular line of code does, it goes after it:

	printf("Hello, world!\n");  // prints "Hello, world!", and a new line

If comments describe more than one line of code, they are placed above the
block of code:

	// initialises the string and prints it out
	char *string = malloc(sizeof (char) * strlen("Hello, world!"));
	strcpy(string, "Hello, world!");
	printf("%s\n", string);

Multi-line comments do, of course, use the syntax used in the comment block.
They should also be capitalised, unlike one-line comments.

A block of code that is already documented previously with a comment, is
marked with a one-line comment designator and a carat (^), using the rules
specified above:

	printf("Hello, world!\n");  // ^

	...

	// ^
	char *string = malloc(sizeof (char) * strlen("Hello, world!"));
	strcpy(string, "Hello, world!");
	printf("%s\n", string);

Variables are initialised, not declared then assigned.

Happy hacking!
*/

#define OPTSTRING "?Vbc:hpsv"

#include "../config.h"

#include <assert.h>					// assert()
#include <ctype.h>					// toupper()
#include <stdio.h>					// printf()
#include <stdint.h>					// uint_fast8_t
#include <string.h>					// strcmp()
#include <getopt.h>					// getopt()

#include "include/btcapi.h"			// rates_t, get_json(), parse_json()
#include "include/cmdlineutils.h"	// help()
#if DEBUG
	#include "include/debug.h"		// debug()
#endif

#include "include/error.h"			// error()

int main(int argc, char **argv) {
	char *api;
	char currency[] = "USD";
	bool got_api = false;
	const struct option long_options[] = {
		{
			.name = "help",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'h'
		},

		{
			.name = "version",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'V'
		},

		{
			.name = "buy",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'b'
		},

		{
			.name = "currency",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 'c'
		},

		{
			.name = "ping",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'p'
		},

		{
			.name = "sell",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 's'
		},

		{
			.name = "verbose",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'v'
		}
	};

	int long_options_i = 0;
	int opt;  // current getopt option
	rates_t rates = {
		.buy = 0.0f,
		.result = false,
		.sell = 0.0f
	};

	bool verbose = false;

	#if DEBUG
	debug("getopt_long()");
	#endif

	while((opt = getopt_long(
		argc,
		argv,
		OPTSTRING,
		long_options,
		&long_options_i
	)) != -1) {
		switch(opt) {
			case '?': case 'h':
				#if DEBUG
				debug("got option 'h'");
				#endif

				#if DEBUG
				debug("help()");
				#endif

				help(argv[0], OPTSTRING);
				break;

			case 'V':
				#if DEBUG
				debug("got option 'V'");
				#endif

				#if DEBUG
				debug("version()");
				#endif

				version(argv[0], BTCWATCH_VERSION);
				break;

			case 'b':
				#if DEBUG
				debug("got option 'b'");
				#endif

				// checks whether API was already processed - saves a lot of time
				if(!got_api) {
					#if DEBUG
					debug("get_json()");
					#endif

					api = get_json(currency, argv[0]);

					#if DEBUG
					debug("get_json()");
					#endif

					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					if(verbose) {
						printf("buy: %f %s\n", rates.buy, currency);

					} else {
						printf("%f\n", rates.buy);
					}

				} else {
					error(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 'c':
				#if DEBUG
				debug("got option 'c'");
				#endif

				strcpy(currency, optarg);

				break;

			case 'p':
				#if DEBUG
				debug("got option 'p'");
				#endif

				// ^
				if(!got_api) {
					#if DEBUG
					debug("get_json()");
					#endif

					api = get_json(currency, argv[0]);

					#if DEBUG
					debug("parse_json()");
					#endif

					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					if(verbose) printf("result: ");
					printf("success\n");
				} else {
					error(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 's':
				#if DEBUG
				debug("got option 's'");
				#endif

				// ^
				if(!got_api) {

					#if DEBUG
					debug("get_json()");
					#endif

					api = get_json(currency, argv[0]);

					#if DEBUG
					debug("parse_json()");
					#endif

					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					if(verbose) {
						printf(
							"sell: %f %s\n",
							rates.sell,
							currency
						);

					} else {
						printf("%f\n", rates.sell);
					}

				} else {
					error(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 'v':
				#if DEBUG
				debug("got option 'v'");
				#endif

				verbose = true;
				break;

			default:
				assert(true == false);
				break;  // just in case
		}
	}

	if(argc == 1) {
		#if DEBUG
		debug("get_json()");
		#endif

		api = get_json(currency, argv[0]);

		#if DEBUG
		debug("parse_json()");
		#endif

		rates = parse_json(api, argv[0]);

		if(rates.result) {
			printf(
				"result: success\n"
				"buy: %f %s\n"
				"sell: %f %s\n",
				rates.buy,
				currency,
				rates.sell,
				currency
			);

		} else {
			error(argv[0], "couldn't get a successful JSON string");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
