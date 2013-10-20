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

#define OPTSTRING "?Vbac:hn:psv"

#include <assert.h>
#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

#include "include/btcapi.h"
#include "include/cmdlineutils.h"
#include "include/btcdbg.h"
#include "include/btcerr.h"

/*
btcapi.c:

rates_t btcrates = {
	.got = false
};
*/

int main(int argc, char **argv) {
	btcdbg(__FILE__, __LINE__, "main()");

	btcerr_t api_err;	// error data structure
	int longopt_i;		// index for referencing long_options[] - needed by getopt_long()
	double n;		// number of BTC to convert
	int opt;  		// current option
	char *pn;  		// pointer to argv[0]
	bool verbose;		// print verbose output?

	char currcy[3 + 1] = "USD";
	const struct option long_options[17] = {
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
			.name = "amount",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 'n'
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

	api_err.err = false;
	n = 1.0;
	pn = argv[0];
	verbose = false;

	setlocale(LC_ALL, "");  // sets the locale to the system's default

	while((opt = getopt_long(
		argc,
		argv,
		OPTSTRING,
		long_options,
		&longopt_i
	)) != -1) {
		switch(opt) {
			case '?': case 'h':  // print this help
				btcdbg(__FILE__, __LINE__, "got option 'h'");

				help(pn, OPTSTRING);
				break;

			case 'V':  // print version number
				btcdbg(__FILE__, __LINE__, "got option 'V'");

				version(BTCWATCH_VERSION);
				break;

			case 'a':  // equivelant to -pbs
				btcdbg(__FILE__, __LINE__, "got option 'a'");

				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);

				if(!api_err.err) {
					if(verbose) {
						puts("result: success");

						resetw();

						wprintf(
							L"buy: %S %f %s\n"
							"sell: %S %f %s\n",
							btcrates.currcy.sign,
							btcrates.buy * n,
							btcrates.currcy.name,
							btcrates.currcy.sign,
							btcrates.sell * n,
							btcrates.currcy.name
						);
						
						resetb();

					} else {
						puts("success");
						printf(
							"%f\n"
							"%f\n",
							btcrates.buy * n,
							btcrates.sell * n
						);

					}
				} else {
					btcerr(pn, "%s", api_err.errstr);
					exit(EXIT_FAILURE);
				}

				break;

			case 'b':  // print buy price
				btcdbg(__FILE__, __LINE__, "got option 'b'");

				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);

				if(!api_err.err) {
					if(verbose) {
						resetw();

						wprintf(
							L"buy: %S %f %s\n",
							btcrates.currcy.sign,
							btcrates.buy * n,
							btcrates.currcy.name
						);

						resetb();

					} else {
						printf("%f\n", btcrates.buy * n);
					}

				} else {
					btcerr(pn, "%s", api_err.errstr);
					exit(EXIT_FAILURE);
				}

				break;

			case 'c':  // set conversion currency
				btcdbg(__FILE__, __LINE__, "got option 'c'");
				btcdbg(__FILE__, __LINE__, "old currcy: \"%s\"", currcy);

				strcpy(currcy, optarg);

				btcdbg(__FILE__, __LINE__, "new currcy: \"%s\"", currcy);

				break;

			case 'n':
				n = atof(optarg);
				break;

			case 'p':  // check for a successful JSON response
				btcdbg(__FILE__, __LINE__, "got option 'p'");

				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);

				if(!api_err.err) {
					if(verbose) {
						fputs("result: ", stdout);
					}

					puts("success");
				} else {
					btcerr(pn, "%s", api_err.errstr);
					exit(EXIT_FAILURE);
				}

				break;

			case 's':  // print sell price
				btcdbg(__FILE__, __LINE__, "got option 's'");

				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);

				if(!api_err.err) {
					if(verbose) {
						resetw();

						wprintf(
							L"sell: %S %f %s\n",
							btcrates.currcy.sign,
							btcrates.sell * n,
							btcrates.currcy.name
						);

						resetb();

					} else {
						printf("%f\n", btcrates.sell * n);
					}
				} else {
					btcerr(pn, "%s", api_err.errstr);
					exit(EXIT_FAILURE);
				}

				break;

			case 'v':  // increase verbosity
				btcdbg(__FILE__, __LINE__, "got option 'v'");

				btcdbg(__FILE__, __LINE__, "verbose = true");

				verbose = true;
				break;

			default:
				exit(EXIT_FAILURE);
				break;  // just in case
		}
	}

	if(argc == 1) {
		fill_rates(currcy, &api_err);

		if(!api_err.err) {
			puts("result: success");

			resetw();

			wprintf(
				L"buy: %S %f %s\n"
				"sell: %S %f %s\n",
				btcrates.currcy.sign,
				btcrates.buy,
				btcrates.currcy.name,
				btcrates.currcy.sign,
				btcrates.sell,
				btcrates.currcy.name
			);

			resetb();

		} else {
			btcerr(pn, "%s", api_err.errstr);
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
