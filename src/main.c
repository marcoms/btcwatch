// btcwatch - gets and monitors Bitcoin trade information

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

#define _(str) gettext(str)
#define BTC_SIGN "฿"
#define STR_TRUE "yes"
#define GREEN(str) "\033[32m" str "\033[0m"
#define RED(str) "\033[31m" str "\033[0m"
#define OPTSTRING "CSVbac:h::k::n:o::prsv::"
#define DEFAULT_MONITOR_INTERVAL 400000  // microseconds (4 seconds)

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
#include <unistd.h>
#include <getopt.h>

#include "lib/libbtcapi/btcapi.h"
#include "include/btcutil.h"
#include "include/config.h"

int main(int argc, char **argv) {
	btc_err_t      api_err;                                      // error data structure
	char           btcpath[64];                                  // path to ~/.btcwatch
	char           btcpathwf[64];                                // path including ".btcstore"
	time_t         btcstore_time;                                // time btcstore was modified
	uint32_t       btcstore_time_tmp;                            // for reading with scanf()
	btc_rates_t    rates;                                        // price of Bitcoin
	btc_rates_t    btcstore;                                     // rates found in ~/.btcwatch/btcstore
	bool           colour           = false;                     // print colour?
	char           currcy[3 + 1]    = "USD";                     // currency to convert to
	bool           found_path       = false;                     // found ~/, ~/.btcwatch, etc?
	bool           forever          = false;                     // keep on monitoring rates?
	FILE          *fp               = NULL;                      // btcstore handle
	int            longopt_i;                                    // index for referencing long_options[]
	uint_fast32_t  monitor_interval = DEFAULT_MONITOR_INTERVAL;  // interval between pinging server when -k is used
	double         n                = 1.0;                       // number of BTC to convert
	char          *newlp            = NULL;                      // pointer to newline
	int            opt;                                          // current option in getopt() loop
	char          *pn               = argv[0];                   // program name - pointer to argv[0]
	bool           reverse          = false;                     // convert currency to Bitcoin?
	char           timestr[32];                                  // string returned by ctime
	bool           verbose          = false;                     // print verbose output?

	// list of options for getopt_long()
	const struct option long_options[] = {
		{
			.name = "help",
			.has_arg = optional_argument,
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
			.name = "keep-monitoring",
			.has_arg = optional_argument,
			.flag = NULL,
			.val = 'k'
		},

		{
			.name = "amount",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 'n'
		},

		{
			.name = "colour",
			.has_arg = optional_argument,
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
			.has_arg = optional_argument,
			.flag = NULL,
			.val = 'v'
		},

		// array terminates here - thanks to Adam Rosenfield (http://stackoverflow.com/questions/20668530)
		{
			0,
			0,
			0,
			0
		}
	};

	api_err.err = false;

	setlocale(LC_ALL, "");  // sets the locale to the system's default

	while((opt = getopt_long(
		argc,
		argv,
		OPTSTRING,
		long_options,
		&longopt_i
	)) != -1) {
		switch(opt) {
			case '?':  // returned by getopt_long() if opt is not in OPTSTRING
			case 'h':
				help(pn, optarg);
				break;

			case 'C':
				if(!found_path) {
					find_paths(btcpath, btcpathwf);
					found_path = true;
				}

				mkdir(btcpath, S_IRWXU);  // checks if ~/.btcwatch exists

				/*
				erno will be set to EEXIST if the directory
				exists (expected behaivior), otherwise the user
				probably removed ~/.btcwatch or hadn't run
				btcwatch -S.
				*/

				if(errno != EEXIST) error(EXIT_FAILURE, 0, "rerun btcwatch with -S");

				/*
				If btcstore is not present but ~/.btcwatch is,
				then the user probably removed it for some
				reason.
				*/

				if((fp = fopen(btcpathwf, "r")) == NULL) error(EXIT_FAILURE, errno, "rerun btcwatch with -S");

				fscanf(fp, "%s", btcstore.currcy.name);
				if(!rates.got || strcmp(rates.currcy.name, btcstore.currcy.name)) api_err = btc_fill_rates(&rates, btcstore.currcy.name);
				if(!api_err.err) {
					// gets the time that btcstore was written to
					fscanf(fp, "%" SCNu32, &btcstore.buy);
					fscanf(fp, "%" SCNu32, &btcstore.sell);
					fscanf(fp, "%" SCNu32, &btcstore_time_tmp);
					btcstore_time = btcstore_time_tmp;
					strcpy(timestr, ctime(&btcstore_time));
					newlp = strchr(timestr, '\n');  // finds newline character
					*newlp = 0;  // strips newline
					btcstore.buyf = ((double) btcstore.buy / (double) rates.currcy.sf);
					btcstore.sellf = ((double) btcstore.sell / (double) rates.currcy.sf);

					if(verbose) {
						if(rates.buy == btcstore.buy) {
							puts("buy: (no change)");
						} else {
							printf(
								"buy: %s %s %f %s (%f -> %f)\n",

								rates.buy > btcstore.buy
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								rates.currcy.sign,
								((double) ((int32_t) rates.buy - (int32_t) btcstore.buy) / rates.currcy.sf),
								rates.currcy.name,
								btcstore.buyf,
								rates.buyf
							);
						}
						if(rates.sell == btcstore.sell) {
							puts("sell: (no change)");
						} else {
							printf(
								"sell: %s %s %f %s (%f -> %f)\n",

								rates.sell > btcstore.sell
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								rates.currcy.sign,
								((double) ((int32_t) rates.sell - (int32_t) btcstore.sell) / rates.currcy.sf),
								rates.currcy.name,
								btcstore.sellf,
								rates.sellf
							);
						}
						fputs("(since ", stdout); fputs(timestr, stdout); puts(")");
					} else {
						if(rates.buy == btcstore.buy) {
							puts("(no change)");
						} else {
							printf(
								"%s %f\n",

								rates.buy > btcstore.buy
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								((double) ((int32_t) rates.buy - (int32_t) btcstore.buy) / rates.currcy.sf)
							);
						}

						if(rates.sell == btcstore.sell) {
							puts("(no change)");
						} else {
							printf(
								"%s %f\n",

								rates.sell > btcstore.sell
									? colour
										? GREEN("UP")
										: "UP"
									: colour
										? RED("DOWN")
										: "DOWN",

								((double) ((int32_t) rates.sell - (int32_t) btcstore.sell) / rates.currcy.sf)
							);
						}
					}
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
				}

				break;

			case 'S':
				// gets user information from /etc/passwd and extracts home directory from it
				if(!found_path) {
					find_paths(btcpath, btcpathwf);
					found_path = true;
				}

				if(mkdir(btcpath, S_IRWXU) == -1 && errno != EEXIST) error(EXIT_FAILURE, errno, "couldn't create ~/.btcwatch");

				fp = fopen(btcpathwf, "w");

				if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
				if(!api_err.err) {
					fprintf(
						fp,
						"%s\n"
						"%" PRId32 "\n"
						"%" PRId32 "\n"
						"%" PRIu32 "\n",
						rates.currcy.name,
						rates.buy,
						rates.sell,
						(uint32_t) time(NULL)
					);
				} else {
					error(EXIT_FAILURE, 0, "%s", api_err.errstr);
				}

				fclose(fp);
				break;

			case 'V':
				version();
				break;

			case 'a':
				if(forever) {
					while(true) {
						if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);  // checks if Bitcoin prices are alreaty obtained or if the user has specified a different currency
						print_rates(&rates, &api_err, P_RESULT | P_BUY | P_SELL, n, verbose, reverse, colour);
						usleep(monitor_interval);
					}
				} else {
						if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
						print_rates(&rates, &api_err, P_RESULT | P_BUY | P_SELL, n, verbose, reverse, colour);
				}

				break;

			case 'b':
				if(forever) {
					while(true) {
						if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
						print_rates(&rates, &api_err, P_BUY, n, verbose, reverse, colour);
						usleep(monitor_interval);
					}
				} else {
					if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
					print_rates(&rates, &api_err, P_BUY, n, verbose, reverse, colour);
				}

				break;

			case 'c':
				strcpy(currcy, optarg);
				break;

			case 'k':
				forever = true;
				if(optarg) monitor_interval = (float) (atof(optarg) * 1000000);
				break;

			case 'n':
				n = atof(optarg);
				break;

			case 'o':
				if(optarg) {
					if(!strcmp(STR_TRUE, optarg)) {
						colour = true;
					} else {
						colour = false;
					}
				} else {
					colour = true;
				}

				break;

			case 'p':
				if(forever) {
					while(true) {
						if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
						print_rates(&rates, &api_err, P_RESULT, n, verbose, reverse, colour);
						usleep(monitor_interval);
					}
				} else {
					if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
					print_rates(&rates, &api_err, P_RESULT, n, verbose, reverse, colour);
				}

				break;

			case 'r':
				reverse = true;
				break;

			case 's':
				if(forever) {
					while(true) {
						if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
						print_rates(&rates, &api_err, P_SELL, n, verbose, reverse, colour);
						sleep(monitor_interval);
					}
				} else {
					if(!rates.got || strcmp(rates.currcy.name, currcy)) api_err = btc_fill_rates(&rates, currcy);
					print_rates(&rates, &api_err, P_SELL, n, verbose, reverse, colour);
				}

				break;

			case 'v':
				if(optarg) {
					if(!strcmp(STR_TRUE, optarg)) {
						verbose = true;
					} else {
						verbose = false;
					}
				} else {
					verbose = true;
				}

				break;

			default:
				error(EXIT_FAILURE, 0, "couldn't parse options");
				break;
		}
	}

	if(argc == 1) {
		// default behavior with no arguments is to print all prices verbosely
		verbose = true;
		api_err = btc_fill_rates(&rates, currcy);
		print_rates(&rates, &api_err, P_RESULT | P_BUY | P_SELL, n, verbose, reverse, colour);
	}

	return EXIT_SUCCESS;
}
