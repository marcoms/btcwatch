AC_INIT([btcwatch], [0.1.3], [marco@scannadinari.co.uk], [btcwatch-0.1.3], [https://github.com/marcoms/btcwatch])

define([errnf], [echo "  configure: error: $1 not found"; exit 1])

AC_PROG_CC
AC_LANG([C])

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

AC_CHECK_LIB([curl], [curl_easy_init], [], [errnf([libcurl])])
AC_CHECK_LIB([jansson], [json_loads], [], [errnf([libjansson])])

AC_OUTPUT

echo
echo
echo "    prefix: $prefix"
echo
echo

cp confdefs.h src/include/config.h

echo $prefix > prefix.txt
