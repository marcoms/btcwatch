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

#define BOLD(str) "\033[1m" str "\033[0m"
#define IGNORE(x) (void) (x)
#define GREEN(str) "\033[32m" str "\033[0m"
#define RED(str) "\033[31m" str "\033[0m"
#define BTC_SIGN "฿"

#include <error.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "include/config.h"
#include "include/btcutil.h"

enum {
	P_RESULT = 0x01,
	P_BUY    = 0x02,
	P_SELL   = 0x04
};

void find_paths(char *const path, char *const pathwf) {
	struct passwd *userinfo;

	userinfo = getpwuid(getuid());
	strcpy(path, userinfo -> pw_dir);
	strcat(path, "/.btcwatch");
	strcpy(pathwf, path);
	strcat(pathwf, "/btcstore");
}

noreturn void help(const char *const prog_nm, const char *const topic) {
	char currcies[][3 + 1] = {
		#ifdef MT_GOX_API
		"AUD",
		"CAD",
		"CHF",
		"CNY",
		"CZK",
		"DKK",
		#endif
		"EUR",
		#ifdef MT_GOX_API
		"GBP",
		"HKD",
		"JPY",
		"NOK",
		"PLN",
		#endif
		#ifdef MT_GOX_API
		"RUB",
		#elif defined(BTC_E_API)
		"RUR",
		#endif
		#ifdef MT_GOX_API
		"SEK",
		"SGD",
		"THB",
		#endif
		"USD",
	};

	char topics[][2][32] = {
		/*{
			"boolean",
			"list possible values for boolean"
		},*/

		{
			"currencies",
			"list available currencies"
		},

		{
			"topics",
			"print this list of topics"
		}
	};

	/*
	help() uses argv[0] for the program name because of the	"Usage:"
	string, where the user expects to be given an explenation on how to use
	and invoke the program. For example, if the program is renamed or
	invoked via a symbolic link, help() will still output a valid "Usage:"
	string.

	version(), on the other hand, merely displays the program's name,
	its version number, and licensing information, all of which should
	remain constant (until the next release).
	*/

	if(!topic) {
		bputs("Usage: "); bputs(prog_nm); puts(" [OPTION]");
		puts(
			"Get and monitor Bitcoin trade information\n"
			"\n"
			"Options:       Long options:\n"
			"  -C             --compare            comare current price with stored price\n"
			"  -S             --store              store current price\n"
			"  -a             --all                equivalent to -pbs\n"
			"  -b             --buy                print buy price\n"
			"  -c CURRENCY    --currency=CURRENCY  set conversion currency\n"
			"  -n AMOUNT      --amount=AMOUNT      set the amount to convert\n"
			"  -o [boolean]   --colour[=boolean]   enable use of colour\n"
			"  -p             --ping               check for a successful JSON response\n"
			"  -r             --reverse            convert currency to Bitcoin\n"
			"  -s             --sell               print sell price\n"
			"  -v [boolean]   --verbose[=boolean]  increase verbosity\n"
			"\n"
			"  -h [topic]     --help[=topic]       print this help, or help designated by topic\n"
			"                                      use --help=topics for available topics\n"
			"  -V             --version            print version number\n"
			"\n"
			"Report bugs to " PACKAGE_BUGREPORT "\n"
			"btcwatch home page: " PACKAGE_URL
		);
		exit(EXIT_SUCCESS);
	} else {
		if(!strcmp(topic, "currencies")) {
			for(
				uint_fast8_t i = 0;
				i < (sizeof currcies / sizeof currcies[0]);
				++i
			) puts(currcies[i]);
			exit(EXIT_SUCCESS);
		} else if(!strcmp(topic, "topics")) {
			for(
				uint_fast8_t i = 0;
				i < (sizeof topics / sizeof topics[0]);
				++i
			) {
				bputs(topics[i][0]); bputs("\t\t"); puts(topics[i][1]);
			}

			exit(EXIT_SUCCESS);
		} else {
			error(EXIT_FAILURE, 0, "no such topic");
		}
	}
	exit(EXIT_SUCCESS);
}

void print_rates(btc_rates_t *rates, btc_err_t *err, uint_fast8_t to_print, uint_fast32_t n, bool verbose, bool reverse, bool colour) {
	if(!(err -> err)) {
		if(verbose) {
			if(to_print & P_RESULT) {
				bputs("result: "); puts(
					colour
						? GREEN("success")
						: "success"
				);
			}

			if(to_print & P_BUY) {
				printf(
					"buy: %s %f %s\n",

					reverse
						? BTC_SIGN
						: rates -> currcy.sign,

					reverse
						? (n / rates -> buyf)
						: ((double) (rates -> buy * n) / (double) rates -> currcy.sf),

					reverse
						? "BTC"
						: rates -> currcy.name
				);
			}

			if(to_print & P_SELL) {
				printf(
					"sell: %s %f %s\n",

					reverse
						? BTC_SIGN
						: rates -> currcy.sign,

					reverse
						? (n / rates -> sellf)
						: ((double) (rates -> sell * n) / (double) rates -> currcy.sf),

					reverse
						? "BTC"
						: rates -> currcy.name
				);
			}
			
		} else {
			if(to_print & P_RESULT) {
				puts(
					colour
						? GREEN("success")
						: "success"
				);
			}

			if(to_print & P_BUY) {
				printf(
					"%f\n",

					reverse
						? (n / rates -> buyf)
						: ((double) (rates -> buy * n) / (double) rates -> currcy.sf)
				);
			}

			if(to_print & P_SELL) {
				printf(
					"%f\n",

					reverse
						? (n / rates -> sellf)
						: ((double) (rates -> sell * n) / (double) rates -> currcy.sf)
				);
			}
		}
	} else {
		error(EXIT_FAILURE, 0, "%s", err -> errstr);
	}
}

noreturn void version(void) {
	printf(
		"%s (%s)\n",
		PACKAGE_STRING,
		#ifdef MT_GOX_API
		"MtGox"
		#elif defined(BTC_E_API)
		"BTC-e"
		#endif
	);
	bputs(
		"Copyright (C) Marco Scannadinari.\n"
		"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law.\n"
		"\n"
		"Written by Marco Scannadinari.\n"
	);

	exit(EXIT_SUCCESS);
}
