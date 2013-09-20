AC_INIT([btcwatch], [0.0.1], [marco@scannadinari.co.uk])

define([errnf], [echo "  ./configure: error: $1 not found"; exit 1])

AC_PROG_CC
AC_LANG([C])

AC_CHECK_TOOL([RANLIB], [ranlib], [:])
AC_CHECK_TOOL([AR], [ar], [:])
AC_CHECK_TOOL([PKGCONFIG], [pkg-config], [:])

AC_CHECK_HEADER([assert.h], [], [errnf([assert.h])])
AC_CHECK_HEADER([stdlib.h], [], [errnf([stdlib.h])])
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
echo "prefix: $prefix"
echo

echo $prefix > prefix.txt
cp confdefs.h config.h
