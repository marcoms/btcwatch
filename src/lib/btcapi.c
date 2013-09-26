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

#include "../../config.h"

#include <curl/curl.h>			// curl_easy_init(), curl_easy_perform(), curl_easy_setopt(), CURLcode, CURLE_OK, CURLOPT_URL, CURLOPT_WRITEDATA, CURLOPT_WRITEFUNCTION
#include <ctype.h>
#include <jansson.h>			// json_error_t, json_loads(), json_object_get(), json_string_value(), json_t
#include <stdbool.h>			// bool, false, true
#include <stdio.h>				// fprintf()
#include <stdint.h>
#include <stdlib.h>				// exit(), EXIT_FAILURE
#include <string.h>				// atof(), strcmp()

#include "../include/btcapi.h"	// get_json(), parse_json(), rates_t
#include "../include/debug.h"	// debug()
#include "../include/error.h"	// error()

char *get_json(char *const currency, const char *const prog_name) {
	char api_url[] = "https://data.mtgox.com/api/2/BTCUSD/money/ticker_fast";
	const char currencies[][3 + 1] = {
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

	#if DEBUG
	debug("curl_easy_init()");
	#endif

	CURL *handle = curl_easy_init();

	#if DEBUG
	debug("malloc()");
	#endif

	char *json = malloc(sizeof (char) * 1600);  // JSON string returned by URL
	CURLcode result;
	bool valid_currency;

	// length check
	if(strlen(currency) != 3) {
		error(prog_name, "invalid currency");
		exit(EXIT_FAILURE);
	}

	// case correction
	for(
		uint_fast8_t i = 0;
		i < ((sizeof currency / sizeof currency[0]) - 1);
		++i
	) currency[i] = toupper(currency[i]);

	// validation
	for(
		uint_fast8_t i = 0;
		i < ((sizeof currencies / sizeof currencies[0]));
		++i
	) {
		if(strcmp(currency, currencies[i]) == 0) {
			valid_currency = true;
			break;
		}
	}

	if(!valid_currency) {
		error(prog_name, "invalid currency");
		exit(EXIT_FAILURE);
	}

	for(
		uint_fast8_t i = API_URL_CURRENCY_POS, j = 0;
		i < (API_URL_CURRENCY_POS + 3);
		++i, ++j
	) api_url[i] = currency[j];

	#if DEBUG
	debug("strncpy()");
	#endif

	#if DEBUG
	debug("curl_global_init()");
	#endif

	curl_global_init(CURL_GLOBAL_ALL);

	if(!handle) {
		error(prog_name, "unable to initialise libcurl session");
		exit(EXIT_FAILURE);
	}

	if(!json) {
		error(prog_name, "unable to allocate memory");
		exit(EXIT_FAILURE);
	}

	#if DEBUG
	debug("curl_easy_setopt()");
	#endif

	curl_easy_setopt(handle, CURLOPT_URL, api_url);

	#if DEBUG
	debug("curl_easy_setopt()");
	#endif

	// sets the function to call
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);

	#if DEBUG
	debug("curl_easy_setopt()");
	#endif

	// sets the data to be given to the function
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, json);

	#if DEBUG
	debug("curl_easy_perform()");
	#endif

	// performs the request, stores result
	result = curl_easy_perform(handle);

	if(result != CURLE_OK) {
		error(prog_name, curl_easy_strerror(result));
		exit(EXIT_FAILURE);
	}

	return json;
}

rates_t parse_json(const char *const json, const char *const prog_name) {
	json_t *buy;
	json_t *data;
	json_error_t json_error;
	rates_t json_rates;

	#if DEBUG
	debug("json_loads()");
	#endif

	json_t *root = json_loads(json, 0, &json_error);

	json_t *sell;

	if(!root) {
		error(prog_name, json_error.text);
		exit(EXIT_FAILURE);
	}

	#if DEBUG
	debug("json_object_get()");
	debug("json_string_value()");
	debug("strcmp()");
	#endif

	json_rates.result = strcmp(json_string_value(json_object_get(root, "result")), "success") ? false : true;

	#if DEBUG
	debug("json_object_get()");
	#endif

	data = json_object_get(root, "data");

	#if DEBUG
	debug("json_object_get()");
	#endif

	buy = json_object_get(data, "buy");

	#if DEBUG
	debug("json_object_get()");
	#endif

	sell = json_object_get(data, "sell");

	#if DEBUG
	debug("json_object_get()");
	debug("json_string_value()");
	debug("atof()");
	#endif

	// stores the buy value as a float
	json_rates.buy = atof(json_string_value(json_object_get(buy, "value")));

	#if DEBUG
	debug("json_object_get()");
	debug("json_string_value()");
	debug("atof()");
	#endif

	// ^
	json_rates.sell = atof(json_string_value(json_object_get(sell, "value")));

	return json_rates;
}

size_t write_data(
	char *buffer,
	size_t size,
	size_t nmemb,
	void *userdata
) {
	#if DEBUG
	debug("strcpy()");
	#endif

	strcpy(userdata, buffer);

	return (size * nmemb);  // needed for libcURL verification
}
