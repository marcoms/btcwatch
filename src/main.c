// btcwatch - gets and monitors Bitcoin trade information

/*
	Copyright (C) 2013  Marco Scannadinari

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

#define _(str) gettext(str)
#define BTC_SIGN L"฿"
#define GREEN(str) "\033[32m" str "\033[0m"
#define RED(str) "\033[31m" str "\033[0m"
#define OPTSTRING "?::CSVbac:n:oprsv"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <inttypes.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

#include "lib/btcapi.h"
#include "include/btcutil.h"
#include "include/config.h"

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
	bool colour;			// print colour?
	char currcy[3 + 1];		// currency to convert to
	bool found_path;		// found ~/, ~/.btcwatch, etc?
	FILE *fp;			// btcstore handle
	int longopt_i;			// index for referencing long_options[]
	double n;			// number of BTC to convert
	char *newlp;			// pointer to newline
	int opt;  			// current option in getopt() loop
	char *pn;  			// *p*rogram *n*ame - pointer to argv[0]
	bool reverse;			// convert currency to Bitcoin?
	char timestr[32];		// string returned by ctime
	bool verbose;			// print verbose output?

	// list of options for getopt_long()
	const struct option long_options[] = {
		{
			.name = "help",
			.has_arg = optional_argument,
			.flag = NULL,
			.val = '?'
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
			.name = "colour",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'o'
		},

		{
			.name = "ping",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'p'
		},

		{
			.name  = "reverse",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'r'
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
	colour = false;
	found_path = false;
	fp = NULL;
	n = 1.0;
	newlp = NULL;
	pn = argv[0];
	reverse = false;
	verbose = false;

	strcpy(currcy, "USD");

	setlocale(LC_ALL, 0);  // sets the locale to the system's default

	while((opt = getopt_long(
		argc,
		argv,
		OPTSTRING,
		long_options,
		&longopt_i
	)) != -1) {
		btcdbg("got option '%c'", opt);
		switch(opt) {
			case '?': case 'h':
				help(pn, optarg);
				break;

			case 'C':


				if(!found_path) {
					find_path(btcpath, btcpathwf);
					found_path = true;
				}

				// checks if ~/.btcwatch exists

				btcdbg("checking %s...", btcpath);
				mkdir(btcpath, S_IRWXU);

				/*
				erno will be set to EEXIST if the directory
				exists (expected behaivior), otherwise the user
				probably removed ~/.btcwatch or hadn't run
				btcwatch -S.
				*/

				if(errno != EEXIST) error(EXIT_FAILURE, 0, "rerun btcwatch with -S");

				// checks if ~/.btcwatch/btcstore exists

				btcdbg("opening %s...", btcpathwf);

				/*
				If btcstore is not present but ~/.btcwatch is,
				then the user probably removed it for some
				reason.
				*/

				if((fp = fopen(btcpathwf, "r")) == NULL) error(EXIT_FAILURE, errno, "rerun btcwatch with -S");

				fscanf(fp, "%s", btcstore.currcy.name);
				if(!btcrates.got || strcmp(btcrates.currcy.name, btcstore.currcy.name) != 0) fill_rates(btcstore.currcy.name, &api_err);
				if(!api_err.err) {
					// gets the time that btcstore was written to
					fscanf(fp, "%" SCNd32, &btcstore.buy);
					fscanf(fp, "%" SCNd32, &btcstore.sell);
					fscanf(fp, "%" SCNu32, &btcstore_time_tmp);
					btcstore_time = btcstore_time_tmp;
					strcpy(timestr, ctime(&btcstore_time));
					newlp = strchr(timestr, '\n');  // finds newline character
					*newlp = 0;  // strips newline
					btcstore.buyf = ((double) btcstore.buy / (double) btcrates.currcy.sf);
					btcstore.sellf = ((double) btcstore.sell / (double) btcrates.currcy.sf);

					if(verbose) {
						if(btcrates.buy == btcstore.buy) {
							puts("buy: (no change)");
						} else {
							resetw();
							wprintf(
								L"buy: %s %S %f %s (%f -> %f)\n",

								btcrates.buy > btcstore.buy
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								btcrates.currcy.sign,
								((double) (btcrates.buy - btcstore.buy) / (double) btcrates.currcy.sf),
								btcrates.currcy.name,
								btcstore.buyf,
								btcrates.buyf
							);
							resetb();
						}
						if(btcrates.sell == btcstore.sell) {
							puts("sell: (no change)");
						} else {
							resetw();
							wprintf(
								L"sell: %s %S %f %s (%f -> %f)\n",

								btcrates.sell > btcstore.sell
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								btcrates.currcy.sign,
								((double) (btcrates.sell - btcstore.sell) / (double) btcrates.currcy.sf),
								btcrates.currcy.name,
								btcstore.sellf,
								btcrates.sellf
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
								"%s %f\n",

								btcrates.buy > btcstore.buy
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								((double) (btcrates.buy - btcstore.buy) / (double) btcrates.currcy.sf)
							);
						}

						if(btcrates.sell == btcstore.sell) {
							puts("(no change)");
						} else {
							printf(
								"%s %f\n",

								btcrates.sell > btcstore.sell
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								((double) (btcrates.sell - btcstore.sell) / (double) btcrates.currcy.sf)
							);
						}
					}
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
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
						"%" PRId32 "\n"
						"%" PRId32 "\n"
						"%" PRIu32 "\n",
						btcrates.currcy.name,
						btcrates.buy,
						btcrates.sell,
						(uint32_t) time(NULL)
					);
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
				}

				btcdbg("closing %s...", btcpathwf);
				fclose(fp);

				break;

			case 'V':
				version();
				break;

			case 'a':
				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);  // checks if Bitcoin prices are alreaty obtained or if the user has specified a different currency 
				if(!api_err.err) {
					if(verbose) {
						puts("result: success");
						resetw();
						wprintf(
							L"buy: %S %f %s\n"
							"sell: %S %f %s\n",

							reverse
								? BTC_SIGN
								: btcrates.currcy.sign,

							reverse
								? (n / btcrates.buyf)
								: ((double) (btcrates.buy * n) / (double) btcrates.currcy.sf),

							reverse
								? "BTC"
								: btcrates.currcy.name,

							reverse
								? BTC_SIGN
								: btcrates.currcy.sign,

							reverse
								? (n / btcrates.sellf)
								: ((double) (btcrates.sell * n) / (double) btcrates.currcy.sf),

							reverse
								? "BTC"
								: btcrates.currcy.name
						);
						resetb();
					} else {
						puts("success");
						printf(
							"%f\n"
							"%f\n",

							reverse
								? (n / btcrates.buyf)
								: ((double) (btcrates.buy * n) / (double) btcrates.currcy.sf),

							reverse
								? (n / btcrates.sellf)
								: ((double) (btcrates.sell * n) / (double) btcrates.currcy.sf)
						);
					}
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
				}

				break;

			case 'b':
				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);
				if(!api_err.err) {
					if(verbose) {
						resetw();
						wprintf(
							L"buy: %S %f %s\n",

							reverse
								? BTC_SIGN
								: btcrates.currcy.sign,

							reverse
								? (n / btcrates.buyf)
								: ((double) (btcrates.buy * n) / (double) btcrates.currcy.sf),

							reverse
								? "BTC"
								: btcrates.currcy.name
						);
						resetb();
					} else {
						printf(
							"%f\n",

							reverse
								? (n / btcrates.buyf)
								: ((double) (btcrates.buy * n) / (double) btcrates.currcy.sf));
					}
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
				}

				break;

			case 'c':
				btcdbg("old currcy: \"%s\"", currcy);
				strcpy(currcy, optarg);
				btcdbg("new currcy: \"%s\"", currcy);

				break;

			case 'n':
				n = atof(optarg);
				break;

			case 'o':
				colour = true;
				break;

			case 'p':
				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);
				if(!api_err.err) {
					if(verbose) {
						fputs("result: ", stdout);
					}
					puts("success");
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
					exit(EXIT_FAILURE);
				}

				break;

			case 'r':
				reverse = true;
				break;

			case 's':
				if(!btcrates.got || strcmp(btcrates.currcy.name, currcy) != 0) fill_rates(currcy, &api_err);
				if(!api_err.err) {
					if(verbose) {
						resetw();
						wprintf(
							L"sell: %S %f %s\n",
							btcrates.currcy.sign,

							reverse
								? (n / btcrates.sellf)
								: ((double) (btcrates.sell * n) / (double) btcrates.currcy.sf),

							btcrates.currcy.name
						);
						resetb();
					} else {
						printf(
							"%f\n",

							reverse
								? (n / btcrates.sellf)
								: ((double) (btcrates.sell * n) / (double) btcrates.currcy.sf)
							);
					}
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
				}

				break;

			case 'v':
				btcdbg("verbose: true");
				verbose = true;
				break;

			default:
				exit(EXIT_FAILURE);
				break;
		}
	}

	if(argc == 1) {
		// default behavior with no arguments is to print all prices verbosely

		fill_rates(currcy, &api_err);
		if(!api_err.err) {
			puts("result: success");
			resetw();
			wprintf(
				L"buy: %S %f %s\n"
				"sell: %S %f %s\n",
				btcrates.currcy.sign,
				btcrates.buyf,
				btcrates.currcy.name,
				btcrates.currcy.sign,
				btcrates.sellf,
				btcrates.currcy.name
			);
			resetb();
		} else {
			error(EXIT_FAILURE, 0, "%s", api_err.errstr);
		}
	}

	return EXIT_SUCCESS;
}
