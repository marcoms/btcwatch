btcwatch
========

[![Join the chat at https://gitter.im/marcoms/btcwatch](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/marcoms/btcwatch?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

btcwatch views and monitors the current exchange rate of the popular cryptocurrency, Bitcoin, in multiple currencies.

Dependencies
------------

* libcurl
* jansson

Installing
----------

### Run the configure script ###

	$ ./configure
	
By default, btcwatch will be installed to /usr/local/. You can spedify a different prefix by setting `--prefix`.

### Build btcwatch ###

You can either build a standard or debug build, and you have a choice of using either MtGox or BTC-e as a source. An example non-debug build using MtGox's API:

	$ CFLAGS="-DMT_GOX_API" make

You must specify one of the two mutually-exclusive APIs as a macro when building btcwatch:

* `MT_GOX_API` - uses MtGox's API
* `BTC_E_API` - uses BTC-E's API

In addition, optimisation is left to the user. For a fairly optimised build, add `-march=native -O2` to your CFLAGS, and for debugging, use `-march=native -Og -g`.

### Install btcwatch ###

Install (as root) either normally, or as a stripped binary:

	# make install

or

	# make install-strip

Using btcwatch
--------------

Using btcwatch is incredibly straightforward. To get buy and sell prices, run btcwatch with no arguments:

	$ btcwatch
	result: success
	buy: $ 820.900000 USD
	sell: $ 820.769000 USD

btcwatch will automatically print verbose output (`-v`) when no arguments are provided, with the currency string at the end of the value, and a identifier and currency sign before.
To only request one item, run btcwatch with one of `-p` (checks for a successful JSON string from MtGox), `-b` (buy price), or `-s` (sell price). The order you specify those options does matter, as that is the order that they will be processed and printed to the screen (this is useful for printing results in multiple currencies), for example:

	$ btcwatch -v -b
	buy: $ 821.000000 USD

	$ btcwatch -v -s -b
	sell: $ 820.900000 USD
	buy: $ 821.000000 USD

	$ btcwatch -v -s -b -p
	sell: $ 820.900000 USD
	buy: $ 821.999000 USD
	result: success

Removing the `-v` (verbose) option will remove extra information from output:

	$ btcwatch -s -b -p
	821.000000
	821.999000
	success

To change the currency from USD to something else, use the `-c` (currency) option:

	$ btcwatch -v -c gbp -b
	buy: £ 520.010000 GBP

	$ btcwatch -v -c jpy -b
	buy: ¥ 86201.000000 JPY

	$ btcwatch -v -c eur -b
	buy: € 611.400000 EUR

You can use uppercase if you want. The currency string is converted to uppercase by btcwatch anyway:

	$ btcwatch -v -c GBP -b
	buy: £ 520.010000 GBP

Specify an amount of Bitcoin to convert with the `-n` option:

	$ btcwatch -v -n 2.5 -b
	buy: $ 2120.848875 USD

By default, btcwatch converts from Bitcoin to the user-set currency (default USD), but you can reverse this by specifying `-r`:

	# how much will 1 USD get me?

	$ btcwatch -r -v -b
	buy: ฿ 0.001179 BTC

	# how about 1K?

	$ btcwatch -n 1000 -r -v -b
	buy: ฿ 1.178773 BTC

You can also store the current price of Bitcoin with `-S`, and compare it later with `-C`:

	# bought some BTC for $100

	$ btcwatch -b
	100.000000

	# stores current price

	$ btcwatch -S
	
	# (after a reasonable amount of time)
	
	$ btcwatch -v -C
	buy: UP $ 100.000000 (100.000000 -> 200.000000)
	sell: UP $ 100.000000 (100.000000 -> 200.000000)
	(Since Thu Jan 1 00:00:00 2013)
	
	# profit!

See what version of btcwatch you are using and what API it uses:

	$ btcwatch -V
	btcwatch 0.1 (MtGox)
	Copyright (C) Marco Scannadinari.
	License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
	This is free software: you are free to change and redistribute it.
	There is NO WARRANTY, to the extent permitted by law.

	Written by Marco Scannadinari.

Cleaning up
-----------

To remove all files generated by make, run:

	$ make clean

To remove the above and all files generated by `./configure` (reverting the source tree to its original state), run:

	$ make distclean

Unsinstalling
-------------

(as root):

	# make uninstall

