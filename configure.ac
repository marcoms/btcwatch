AC_INIT([btcwatch], [0.0.1], [marco@scannadinari.co.uk])

define([errnf], [echo "  ./configure: error: cannot find $1"; exit 1])

AC_PROG_CC
AC_LANG([C])

AC_CHECK_FILE([/], [], [errnf([/])])
AC_CHECK_FILE([./], [], [errnf([./])])
AC_CHECK_FILE([src/], [], [errnf([src/])])
AC_CHECK_FILE([src/include/], [], [errnf([src/include/])])
AC_CHECK_FILE([src/lib/], [], [errnf([src/lib/])])

AC_CHECK_FILE([src/include/btcapi.h], [], [errnf([src/include/btcapi.h])])
AC_CHECK_FILE([src/include/cmdlineutils.h], [], [errnf([src/include/cmdlineutils.h])])
AC_CHECK_FILE([src/include/err.h], [], [errnf([src/include/err.h])])

AC_CHECK_FILE([src/main.c], [], [errnf([src/main.c])])
AC_CHECK_FILE([src/lib/btcapi.c], [], [errnf([src/lib/btcapi.c])])
AC_CHECK_FILE([src/lib/cmdlineutils.c], [], [errnf([src/lib/cmdlineutils.c])])

AC_CHECK_HEADER([assert.h], [], [errnf([assert.h])])
AC_CHECK_HEADER([ctype.h], [], [errnf([ctype.h])])
AC_CHECK_HEADER([stdlib.h], [], [errnf([stdlib.h])])
AC_CHECK_HEADER([stdint.h], [], [errnf([stdint.h])])
AC_CHECK_HEADER([stdio.h], [], [errnf([stdio.h])])
AC_CHECK_HEADER([stdbool.h], [], [errnf([stdio.h])])
AC_CHECK_HEADER([curl/curl.h], [], [errnf([curl/curl.h])])
AC_CHECK_HEADER([getopt.h], [], [errnf([getopt.h])])
AC_CHECK_HEADER([jansson.h], [], [errnf([jansson.h])])
AC_CHECK_HEADER([string.h], [], [errnf([string.h])])

AC_CHECK_LIB([c], [printf], [], [errnf([libc])])
AC_CHECK_LIB([curl], [curl_easy_init], [], [errnf([libcurl])])
AC_CHECK_LIB([jansson], [json_loads], [], [errnf([libjansson])])

AC_OUTPUT

echo
echo "prefix:   $prefix"

echo $prefix > prefix.txt

cp confdefs.h config.h
