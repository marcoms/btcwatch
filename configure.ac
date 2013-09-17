AC_INIT([btcwatch], [0.0.1], [marco@scannadinari.co.uk])

AC_PROG_CC
AC_LANG([C])

AC_CHECK_TOOL([RANLIB], [ranlib], [:])
AC_CHECK_TOOL([AR], [ar], [:])
AC_CHECK_TOOL([PKGCONFIG], [pkg-config], [:])

AC_CHECK_HEADERS([stdlib.h stdio.h stdbool.h curl/curl.h jansson.h string.h])
AC_CHECK_LIB([c], printf)
AC_CHECK_LIB([curl], curl_easy_init)
AC_CHECK_LIB([jansson], json_loads)

AC_OUTPUT
cp confdefs.h config.h
