AC_INIT([btcwatch], [0.1.4], [marco@scannadinari.co.uk], [btcwatch-0.1.4], [https://github.com/marcoms/btcwatch])

define([errnf], [echo "  configure: error: $1 not found"; exit 1])

AC_PROG_CC
AC_LANG([C])

AC_CHECK_HEADER([curl/curl.h], [], [errnf([curl/curl.h])])
AC_CHECK_HEADER([getopt.h], [], [errnf([getopt.h])])
AC_CHECK_HEADER([jansson.h], [], [errnf([jansson.h])])
AC_CHECK_HEADER([unistd.h], [], [errnf([unistd.h])])

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
