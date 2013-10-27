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

#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include "../include/config.h"
#include "../include/btcutil.h"
#include "../include/btcdbg.h"

void find_path(char *path, char *pathwf) {
	btcdbg("find_path()");

	struct passwd *userinfo;

	userinfo = getpwuid(getuid());
	strcpy(path, userinfo -> pw_dir);
	strcat(path, "/.btcwatch");
	strcpy(pathwf, path);
	strcat(pathwf, "/btcstore");

	btcdbg("~/: %s", userinfo -> pw_dir);
	btcdbg("~/.btcwatch: %s", path);
	btcdbg("~/.btcwatch/btcstore: %s", pathwf);
}

noreturn void help(const char *const prog_name, const char *const optstring) {
	btcdbg("help()");

	printf("Usage: %s -[%s]\n", prog_name, optstring);
	puts(
		"Get Bitcoin trade information\n"
		"\n"
		"Options:\n"
		"  -C             --compare              comare current price with stored price\n"
		"  -S             --store                store current price\n"
		"  -a             --all                  equivalent to -pbs\n"
		"  -b             --buy                  print buy price\n"
		"  -c CURRENCY    --currency=CURRENCY    set conversion currency\n"
		"  -n AMOUNT      --amount=AMOUNT        sets the amount of Bitcoin to convert\n"
		"  -p             --ping                 check for a successful JSON response\n"
		"  -s             --sell                 print sell price\n"
		"  -v             --verbose              increase verbosity\n"
		"\n"
		"  -h, -?         --help                 print this help\n"
		"  -V             --version              print version number\n"
		"\n"
		"Report bugs to marco@scannadinari.co.uk\n"
		"btcwatch home page: <https://github.com/marcoms/btcwatch/>"
	);

	exit(EXIT_SUCCESS);
}

void resetb(void) {
	btcdbg("resetb()");

	freopen(NULL, "a", stdout);  // reopen stdout
	fwide(stdout, -1);  // set stdout to be byte-oriented
}

void resetw(void) {
	btcdbg("resetw()");

	freopen(NULL, "a", stdout);  // ^
	fwide(stdout, 1);  // set stdout to be wide-oriented
}

noreturn void version(void) {
	btcdbg("version()");

	printf("%s\n", PACKAGE_STRING);
	puts(
		"Copyright (C) Marco Scannadinari.\n"
		"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law.\n"
		"\n"
		"Written by Marco Scannadinari."
	);
	
	#if DEBUG  // else CC wouldn't be defined
	btcdbg("compiled with %s on %s %s", CC, __TIME__, __DATE__);
	#endif

	exit(EXIT_SUCCESS);
}
