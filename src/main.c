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

#define OPTSTRING "?Vbac:hpsv"

#include "../config.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

#include "include/btcapi.h"
#include "include/cmdlineutils.h"
#include "include/debug.h"

#include "include/error.h"

int main(int argc, char **argv) {
	char currency[] = "USD";
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
			.name = "all",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'a'
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
	char *const pn = argv[0];
	bool verbose = false;

	#if DEBUG
	debug(__FILE__, __LINE__, "getopt_long()");
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
				debug(__FILE__, __LINE__, "got option 'h'");
				#endif

				#if DEBUG
				debug(__FILE__, __LINE__, "help()");
				#endif

				help(pn, OPTSTRING);
				break;

			case 'V':
				#if DEBUG
				debug(__FILE__, __LINE__, "got option 'V'");
				#endif

				#if DEBUG
				debug(__FILE__, __LINE__, "version()");
				#endif

				version(pn, BTCWATCH_VERSION);
				break;

			case 'a':
				#if DEBUG
				debug(__FILE__, __LINE__, "got option 'a'");
				#endif

				if(ping(pn)) {
					if(verbose) {
						printf(
							"result: success\n"
							"buy: %f %s\n"
							"sell: %f %s\n",
							buy(currency, pn),
							currency,
							sell(currency, pn),
							currency
						);

					} else {
						printf(
							"success"
							"%f\n"
							"%f\n",
							buy(currency, pn),
							sell(currency, pn)
						);

					}
				} else {
					error(pn, "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

			case 'b':
				#if DEBUG
				debug(__FILE__, __LINE__, "got option 'b'");
				#endif

				if(ping(pn)) {
					if(verbose) {
						printf("buy: %f %s\n", buy(currency, pn), currency);

					} else {
						printf("%f\n", buy(currency, pn));
					}

				} else {
					error(pn, "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 'c':
				#if DEBUG
				debug(__FILE__, __LINE__, "got option 'c'");
				#endif

				strcpy(currency, optarg);

				break;

			case 'p':
				#if DEBUG
				debug(__FILE__, __LINE__, "got option 'p'");
				#endif

				if(ping(pn)) {
					if(verbose) {
						printf("result: ");
					}

					puts("success");
				} else {
					error(pn, "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 's':
				#if DEBUG
				debug(__FILE__, __LINE__, "got option 's'");
				#endif

				if(ping(pn)) {
					if(verbose) {
						printf(
							"sell: %f %s\n",
							sell(currency, pn),
							currency
						);

					} else {
						printf("%f\n", sell(currency, pn));
					}

				} else {
					error(pn, "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 'v':
				#if DEBUG
				debug(__FILE__, __LINE__, "got option 'v'");
				#endif

				verbose = true;
				break;

			default:
				assert(true == false);
				break;  // just in case
		}
	}

	if(argc == 1) {
		if(ping(pn)) {
			printf(
				"result: success\n"
				"buy: %f %s\n"
				"sell: %f %s\n",
				buy(currency, pn),
				currency,
				sell(currency, pn),
				currency
			);

		} else {
			error(pn, "couldn't get a successful JSON string");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
