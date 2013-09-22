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

#define API_URL_CURRENCY_POS 32

#include "../config.h"

#if HAVE_LIBC
	#include <assert.h>				// assert()
	#include <ctype.h>				// toupper()
	#include <stdio.h>				// printf()
	#include <stdint.h>				// uint_fast8_t
	#include <string.h>				// strcmp()
	#include <getopt.h>				// getopt()
#else
	#error libc not found
#endif

#include "include/btcapi.h"			// rates_t, get_json(), parse_json()
#include "include/cmdlineutils.h"	// help()
#include "include/errutils.h"		// ERR()

int main(int argc, char** argv) {
	char *api;
	char api_url[] = "https://data.mtgox.com/api/2/BTCUSD/money/ticker_fast";
	char currencies[][3 + 1] = {
		"AUD",
		"CAD",
		"CHF",
		"CNY",
		"CZK",
		"DKK",
		"EUR",
		"GBP",
		"HKD",
		"JPY",
		"NOK",
		"PLN",
		"RUB",
		"SEK",
		"SGD",
		"THB",
		"USD"
	};

	char currency[] = "USD";
	char currency_arg[3 + 1];
	bool got_api = false;
	int opt;						// current getopt option
	char optstring[] = "?Vbc:hpsv";	// string of valid options
	rates_t rates = {
		.buy = 0.0f,
		.result = false,
		.sell = 0.0f
	};

	bool valid_currency = false;
	bool verbose = false;

	while((opt = getopt(
		argc,
		argv,
		optstring
	)) != -1) {
		switch(opt) {
			case '?': case 'h':
				help(argv[0], optstring);
				break;

			case 'V':
				version(argv[0], BTCWATCH_VERSION);

				break;

			case 'b':
				// checks whether API was already processed - saves a lot of time
				if(!got_api) {
					api = get_json(api_url, argv[0]);
					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					if(verbose) {
						printf(
							"buy: %f %s\n",
							rates.buy,
							currency
						);

					} else {
						printf("%f\n", rates.buy);
					}

				} else {
					ERR(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 'c':
				// a currency must be 3 characters long
				if(strlen(optarg) != 3) {
					if(verbose) {
						ERR(argv[0], "invalid currency - must be three characters long");
					} else {
						ERR(argv[0], "invalid currency");
					}

					exit(EXIT_FAILURE);
				}

				// copies the next argument - the desired currency - to currency_arg
				strncpy(currency_arg, optarg, (sizeof currency_arg / sizeof currency_arg[0]));

				// converts each character excluding the NUL character to its uppercase equivelant
				for(
					uint_fast8_t i = 0;
					i < ((sizeof currency_arg / sizeof currency_arg[0]) - 1);
					++i  // increments i
				) currency_arg[i] = toupper(currency_arg[i]);

				// checks for a valid currency
				for(
					uint_fast8_t i = 0;
					i < ((sizeof currencies / sizeof currencies[0]));
					++i
				) {
					if(!strcmp(currency_arg, currencies[i])) {
						valid_currency = true;
						break;
					}
				}

				if(!valid_currency) {
					if(verbose) {
						ERR(argv[0], "invalid currency: not supported by MtGox");
					} else {
						ERR(argv[0], "invalid currency");
					}

					exit(EXIT_FAILURE);
				}

				for(
					uint_fast8_t i = API_URL_CURRENCY_POS, j = 0;
					i < (API_URL_CURRENCY_POS + 3);
					++i, ++j
				) api_url[i] = currency_arg[j];

				strncpy(currency, currency_arg, (sizeof currency / sizeof currency[0]));

				break;

			case 'p':
				//
				if(!got_api) {
					api = get_json(api_url, argv[0]);
					rates = parse_json(api, argv[0]);

					got_api = true;
				}

				if(rates.result) {
					if(verbose) printf("result: ");
					printf("success\n");
				} else {
					ERR(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 's':
				//
				if(!got_api) {
					api = get_json(api_url, argv[0]);
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
					ERR(argv[0], "couldn't get a successful JSON string");
					exit(EXIT_FAILURE);
				}

				break;

			case 'v':
				verbose = true;
				break;

			default:
				assert(true == false);

				break;  // just in case
		}
	}

	if(argc == 1) {
		verbose = true;

		api = get_json(api_url, argv[0]);
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
			ERR(argv[0], "couldn't get a successful JSON string");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
