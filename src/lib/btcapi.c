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

#include <curl/curl.h>
#include <ctype.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/btcapi.h"
#include "../include/debug.h"
#include "../include/error.h"

bool got_mtgox_rates = false;
rates_t mtgox_rates = {
	.buy = 0.0f,
	.result = false,
	.sell = 0.0f
};

float buy(char *const currency, const char *const prog_name) {
	char *mtgox_json;

	if(!got_mtgox_rates) {
		#if DEBUG
		debug(__FILE__, __LINE__, "get_json()");
		#endif

		mtgox_json = get_json(currency, prog_name); // g

		#if DEBUG
		debug(__FILE__, __LINE__, "parse_json()");
		#endif

		mtgox_rates = parse_json(mtgox_json, prog_name);
		got_mtgox_rates = true;
	}

	#if DEBUG
	debug(__FILE__, __LINE__, "mtgox_rates: b%f r%d s%f", mtgox_rates.buy, mtgox_rates.result, mtgox_rates.sell);
	#endif

	return mtgox_rates.buy;
}

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
	char mod_currency[3 + 1];

	#if DEBUG
	debug(__FILE__, __LINE__, "curl_easy_init()");
	#endif

	CURL *handle = curl_easy_init();
	curl_global_init(CURL_GLOBAL_ALL);

	#if DEBUG
	debug(__FILE__, __LINE__, "malloc()");
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
	) mod_currency[i] = toupper(currency[i]);

	// validation
	for(
		uint_fast8_t i = 0;
		i < ((sizeof currencies / sizeof currencies[0]));
		++i
	) {
		if(strcmp(mod_currency, currencies[i]) == 0) {
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
	) {
		api_url[i] = mod_currency[j];
	}

	#if DEBUG
	debug(__FILE__, __LINE__, "strncpy()");
	#endif

	#if DEBUG
	debug(__FILE__, __LINE__, "curl_global_init()");
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
	debug(__FILE__, __LINE__, "curl_easy_setopt()");
	#endif

	curl_easy_setopt(handle, CURLOPT_URL, api_url);

	#if DEBUG
	debug(__FILE__, __LINE__, "curl_easy_setopt()");
	#endif

	// sets the function to call
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);

	#if DEBUG
	debug(__FILE__, __LINE__, "curl_easy_setopt()");
	#endif

	// sets the data to be given to the function
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, json);

	#if DEBUG
	debug(__FILE__, __LINE__, "curl_easy_perform()");
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
	debug(__FILE__, __LINE__, "json_loads()");
	#endif

	json_t *root = json_loads(json, 0, &json_error);

	json_t *sell;

	if(!root) {
		error(prog_name, json_error.text);
		exit(EXIT_FAILURE);
	}

	#if DEBUG
	debug(__FILE__, __LINE__, "json_object_get()");
	debug(__FILE__, __LINE__, "json_string_value()");
	debug(__FILE__, __LINE__, "strcmp()");
	#endif

	json_rates.result = strcmp(json_string_value(json_object_get(root, "result")), "success") ? false : true;

	#if DEBUG
	debug(__FILE__, __LINE__, "json_object_get()");
	#endif

	data = json_object_get(root, "data");

	#if DEBUG
	debug(__FILE__, __LINE__, "json_object_get()");
	#endif

	buy = json_object_get(data, "buy");

	#if DEBUG
	debug(__FILE__, __LINE__, "json_object_get()");
	#endif

	sell = json_object_get(data, "sell");

	#if DEBUG
	debug(__FILE__, __LINE__, "json_object_get()");
	debug(__FILE__, __LINE__, "json_string_value()");
	debug(__FILE__, __LINE__, "atof()");
	#endif

	// stores the buy value as a float
	json_rates.buy = atof(json_string_value(json_object_get(buy, "value")));

	debug(__FILE__, __LINE__, "buy %f", json_rates.buy);

	#if DEBUG
	debug(__FILE__, __LINE__, "json_object_get()");
	debug(__FILE__, __LINE__, "json_string_value()");
	debug(__FILE__, __LINE__, "atof()");
	#endif

	// ^
	json_rates.sell = atof(json_string_value(json_object_get(sell, "value")));

	debug(__FILE__, __LINE__, "sell %f", json_rates.sell);

	return json_rates;
}

bool ping(char *const prog_name) {
	char *mtgox_json;

	if(!got_mtgox_rates) {
		#if DEBUG
		debug(__FILE__, __LINE__, "get_json()");
		#endif

		mtgox_json = get_json("USD", prog_name); // g

		#if DEBUG
		debug(__FILE__, __LINE__, "parse_json()");
		#endif

		mtgox_rates = parse_json(mtgox_json, prog_name);
		got_mtgox_rates = true;
	}

	#if DEBUG
	debug(__FILE__, __LINE__, "mtgox_rates: b%f r%d s%f", mtgox_rates.buy, mtgox_rates.result, mtgox_rates.sell);
	#endif

	return mtgox_rates.result;
}

float sell(char *const currency, const char *const prog_name) {
	char *mtgox_json;

	if(!got_mtgox_rates) {
		#if DEBUG
		debug(__FILE__, __LINE__, "get_json()");
		#endif

		mtgox_json = get_json(currency, prog_name); // g

		#if DEBUG
		debug(__FILE__, __LINE__, "parse_json()");
		#endif

		mtgox_rates = parse_json(mtgox_json, prog_name);
		got_mtgox_rates = true;
	}

	#if DEBUG
	debug(__FILE__, __LINE__, "mtgox_rates: b%f r%d s%f", mtgox_rates.buy, mtgox_rates.result, mtgox_rates.sell);
	#endif

	return mtgox_rates.sell;
}

size_t write_data(
	char *buffer,
	size_t size,
	size_t nmemb,
	void *userdata
) {
	#if DEBUG
	debug(__FILE__, __LINE__, "strcpy()");
	#endif

	strcpy(userdata, buffer);

	return (size * nmemb);  // needed for libcURL
}
