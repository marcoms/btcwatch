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

Happy hacking!
*/

#define OPTSTRING "?CSVbac:hn:psv"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

#include "include/btcapi.h"
#include "include/btcutil.h"
#include "include/config.h"
#include "include/btcdbg.h"
#include "include/btcerr.h"

/*
btcapi.c:

rates_t btcrates = {
	.got = false
};
*/

int main(int argc, char **argv) {
	btcdbg("main()");

	btcerr_t api_err;		// error data structure
	char btcpath[64];		// path to ~/.btcwatch
	char btcpathwf[64];		// path including ".btcstore"
	time_t btcstore_time;		// time btcstore was modified
	uint32_t btcstore_time_tmp;	// for reading with scanf()
	rates_t btcstore;		// rates found in ~/.btcwatch/btcstore
	char currcy[3 + 1];		// currency to convert to
	bool found_path;		// found ~/, ~/.btcwatch, etc?
	FILE *fp;			// .btcstore handle
	int longopt_i;			// index for referencing long_options[]
	double n;			// number of BTC to convert
	char *newlp;			// pointer to newline
	int opt;  			// current option in getopt() loop
	char *pn;  			// pointer to argv[0]
	char timestr[32];		// string returned by ctime
	bool verbose;			// print verbose output?

	const struct option long_options[17] = {
		{
			.name = "help",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'h'
		},

		{
			.name = "compare",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'C'
		},

		{
			.name = "store",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'S'
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
	found_path = false;
	n = 1.0;
	pn = argv[0];
	verbose = false;

	strcpy(currcy, "USD");

	setlocale(LC_ALL, "");  // sets the locale to the system's default

	while((opt = getopt_long(
		argc,
		argv,
		OPTSTRING,
		long_options,
		&longopt_i
	)) != -1) {
		btcdbg("got option '%c'", opt);

		switch(opt) {
			case '?': case 'h':  // print this help
				help(pn, OPTSTRING);
				break;

			case 'C':
				if(!found_path) {
					find_path(btcpath, btcpathwf);
					found_path = true;
				}

				// checks if ~/.btcwatch exists

				btcdbg("checking %s...", btcpath);
				mkdir(btcpath, S_IRWXU);
				if(errno != EEXIST) {
					btcerr(pn, "No price stored - rerun btcwatch with -S");
					exit(EXIT_FAILURE);
				}

				// checks if ~/.btcwatch/btcstore exists

				btcdbg("opening %s...", btcpathwf);
				if((fp = fopen(btcpathwf, "r")) == NULL) {
					btcerr(pn, "%s - rerun btcwatch with -S", strerror(errno));
					exit(EXIT_FAILURE);
				}

				fscanf(fp, "%s", btcstore.currcy.name);
				if(!btcrates.got || strcmp(btcrates.currcy.name, btcstore.currcy.name) != 0) fill_rates(btcstore.currcy.name, &api_err);
				if(!api_err.err) {
					// gets the time that btcstore was written to

					fscanf(fp, "%lf", &btcstore.buy);
					fscanf(fp, "%lf", &btcstore.sell);
					fscanf(fp, "%" SCNu32, &btcstore_time_tmp);
					btcstore_time = btcstore_time_tmp;
					strcpy(timestr, ctime(&btcstore_time));
					newlp = strchr(timestr, '\n');
					*newlp = '\0';  // strips newline

					btcdbg("rates bdiff: %f", btcrates.buy - btcstore.buy);
					btcdbg("rates sdiff: %f", btcrates.sell - btcstore.sell);

					if(verbose) {
						if(btcrates.buy == btcstore.buy) {
							puts("buy: (no change)");
						} else {
							resetw();
							wprintf(
								L"buy: %s by %S %f %s (%f -> %f)\n",
								((btcrates.buy > btcstore.buy) ? "UP" : "DOWN"),
								btcrates.currcy.sign,
								(btcrates.buy - btcstore.buy),
								btcrates.currcy.name,
								btcstore.buy,
								btcrates.buy
							);
							resetb();
						}
						if(btcrates.sell == btcstore.sell) {
							puts("sell: (no change)");
						} else {
							resetw();
							wprintf(
								L"sell: %s by %S %f %s (%f -> %f)\n",
								((btcrates.sell > btcstore.sell) ? "UP" : "DOWN"),
								btcrates.currcy.sign,
								(btcrates.sell - btcstore.sell),
								btcrates.currcy.name,
								btcstore.sell,
								btcrates.sell
							);
							resetb();
						}
						fputs("(since ", stdout);
						fputs(timestr, stdout);
						puts(")");
					} else {
						if(btcrates.buy == btcstore.buy) {
							puts("(no change)");
						} else {
							printf(
								"%s by %f\n",
								((btcrates.buy > btcstore.buy) ? "UP" : "DOWN"),
								(btcrates.buy - btcstore.buy)
							);
						}

						if(btcrates.sell == btcstore.sell) {
							puts("(no change)");
						} else {
							printf(
								"%s by %f\n",
								((btcrates.sell > btcstore.sell) ? "UP" : "DOWN"),
								(btcrates.sell - btcstore.sell)
							);
						}
					}
				} else {
					btcerr(pn, "%s", api_err.errstr);
					exit(EXIT_FAILURE);
				}

				btcdbg("closing %s...", btcpathwf);

				break;

			case 'S':
				// gets user information from /etc/passwd and extracts home directory from it

				if(!found_path) {
					find_path(btcpath, btcpathwf);
					found_path = true;
				}
				btcdbg("possibly creating %s...", btcpath);
				if(mkdir(btcpath, S_IRWXU) == -1) btcdbg("mkdir(): %s", strerror(errno));
				btcdbg("creating/opening %s...", btcpathwf);
				fp = fopen(btcpathwf, "w");

				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);
				if(!api_err.err) {
					fprintf(
						fp,
						"%s\n"
						"%f\n"
						"%f\n"
						"%" PRIu32 "\n",
						btcrates.currcy.name,
						btcrates.buy,
						btcrates.sell,
						(uint32_t) time(NULL)
					);
				} else {
					btcerr(pn, "%s", api_err.errstr);
					exit(EXIT_FAILURE);
				}

				btcdbg("closing %s...", btcpathwf);
				fclose(fp);

				break;

			case 'V':  // print version number
				version();
				break;

			case 'a':  // equivelant to -pbs
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
						puts("Success");
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
				btcdbg("old currcy: \"%s\"", currcy);
				strcpy(currcy, optarg);
				btcdbg("new currcy: \"%s\"", currcy);

				break;

			case 'n':
				n = atof(optarg);
				break;

			case 'p':  // check for a successful JSON response
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
				btcdbg("verbose: true");
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
