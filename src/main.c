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
	#include <assert.h>				// assert
	#include <stdio.h>				// printf
	#include <string.h>				// strcmp
	#include <getopt.h>				// getopt
#else
	#error libc not found
#endif

#include "include/btcapi.h"			// rates_t, get_api, parse_json
#include "include/cmdlineutils.h"	// help
#include "include/errutils.h"		// ERR

int main(int argc, char** argv) {
	char *api;
	/*char currency[] = "USD";
	char *currency_arg = NULL;
	char currencies[][3 + 1] = {
		"AUD",
		"CAD",
		"CHF",
		"CNY",
		"CZK",
		"DKK",
		"EUR",
		"GBP",       will implement soon!
		"HKD",
		"JPY",
		"NOK",
		"PLN",
		"RUB",
		"SEK",
		"SGD",
		"THB",
		"USD"
	};*/

	bool got_api = false;
	int opt;						// current getopt option
	char optstring[] = "?bchpsv";	// string of valid options
	rates_t rates;

	while((opt = getopt(argc, argv, optstring)) != -1) {
		switch(opt) {
			case '?': case 'h':
				help(argv[0], optstring);

				break;

			case 'b':
				if(!got_api) {
					api = get_api(URL_API, argv[0]);
					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					printf("%f\n", rates.buy);
				} else {
					ERR(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			//case 'c':
				// printf("HI");

				/*currency_arg = optarg;

				printf("%s\n", currency_arg);

				for(unsigned short int i = 0; i < sizeof currencies / sizeof currencies[0]; ++i) {
					if(strcmp(currency_arg, currencies[i])) {
						ERR(argv[0], "invalid currency");
						exit(EXIT_FAILURE);
					}
				}
*/
				//break;

			case 'p':
				if(!got_api) {
					api = get_api(URL_API, argv[0]);
					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					printf("success\n");
				} else {
					ERR(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 's':
				if(!got_api) {
					api = get_api(URL_API, argv[0]);
					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					printf("%f\n", rates.sell);
				} else {
					ERR(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 'v':
				version(argv[0], "0.0.1");

				break;

			default:
				assert(true == false);

				break;  // just in case
		}
	}

	if(argc == 1) {
		api = get_api(URL_API, argv[0]);
		rates = parse_json(api, argv[0]);

		if(rates.result) {
			printf("success\n");
		} else {
			ERR(argv[0], "couldn't get a successful JSON string");
			exit(EXIT_FAILURE);
		}
	}
		

	return 0;
}
