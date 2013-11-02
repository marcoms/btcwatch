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

#define API_URL_CURRCY_POS 32  // position to insert currency

#include <curl/curl.h>
#include <ctype.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/btcapi.h"
#include "../include/btcutil.h"

rates_t btcrates = {
	.got = false
};

bool fill_rates(const char *const currcy, btcerr_t *const api_err) {
	btcdbg("fill_rates()");

	char *json;

	json = get_json(currcy, api_err);
	if(api_err -> err) {
		free(json);
		return false;
	}

	parse_json(json, api_err);
	if(api_err -> err) {
		free(json);
		return false;
	}

	free(json);
	return true;
}

char *get_json(const char *const currcy, btcerr_t *const api_err) {
	btcdbg("get_json()");

	char api_url[] = "https://data.mtgox.com/api/2/BTCxxx/money/ticker_fast";
	currcy_t currencies[] = {
		// australia

		{
			.name = "AUD",
			.sign = L"$",
			.sf = (1e5)
		},

		// canada

		{
			.name = "CAD",
			.sign = L"$",
			.sf = (1e5)
		},

		// switzerland

		{
			.name = "CHF",
			.sign = L"Fr.",
			.sf = (1e5)
		},

		// china

		{
			.name = "CNY",
			.sign = L"¥",
			.sf = (1e5)
		},

		// czech republic

		{
			.name = "CZK",
			.sign = L"Kč.",
			.sf = (1e5)
		},

		// denmark

		{
			.name = "DKK",
			.sign = L"kr.",
			.sf = (1e5)
		},

		// italy

		{
			.name = "EUR",
			.sign = L"€",
			.sf = (1e5)
		},

		// great britain

		{
			.name = "GBP",
			.sign = L"£",
			.sf = (1e5)
		},

		// hong kong

		{
			.name = "HKD",
			.sign = L"$",
			.sf = (1e5)
		},

		// japan

		{
			.name = "JPY",
			.sign = L"¥",
			.sf = (1e3)
		},

		// norway

		{
			.name = "NOK",
			.sign = L"kr.",
			.sf = (1e5)
		},

		// poland

		{
			.name = "PLN",
			.sign = L"zł.",
			.sf = (1e5)
		},

		// russia

		{
			.name = "RUB",
			.sign = L"p.",
			.sf = (1e5)
		},

		// sweden

		{
			.name = "SEK",
			.sign = L"kr.",
			.sf = (1e3)
		},

		// singapore

		{
			.name = "SGD",
			.sign = L"$",
			.sf = (1e5)
		},

		// thailand

		{
			.name = "THB",
			.sign = L"฿",
			.sf = (1e5)
		},

		// united states

		{
			.name = "USD",
			.sign = L"$",
			.sf = (1e5)
		},
	};

	char *json;
	char mod_currcy[3 + 1];
	CURL *handle;
	CURLcode result;
	bool valid_currcy;

	strcpy(mod_currcy, currcy);

	json = malloc(sizeof (char) * 1600);
	if(json == NULL) {
		btcdbg("can't allocate memory");
		abort();
	}

	handle = curl_easy_init();
	if(!handle) {
		api_err -> err = true;
		strcpy(api_err -> errstr, "unable to initialise libcurl session");
		return NULL;
	}

	valid_currcy = false;

	curl_global_init(CURL_GLOBAL_ALL);

	btcdbg("currcy \"%s\" strlen: %d", currcy, strlen(currcy));

	// length check

	if(strlen(currcy) != 3) {
		btcdbg("bad currency length");
		api_err -> err = true;
		strcpy(api_err -> errstr, "bad currency length");
		return NULL;
	}

	// uppercases the currency string

	for(
		uint_fast8_t i = 0;
		i < (sizeof mod_currcy[0] * sizeof mod_currcy);
		++i
	) mod_currcy[i] = toupper(mod_currcy[i]);

	btcdbg("mod_currcy \"%s\" strlen: %d", mod_currcy, strlen(mod_currcy));

	// validation

	for(
		uint_fast8_t i = 0;
		i < ((sizeof currencies / sizeof currencies[0]));
		++i
	) {
		if(strcmp(mod_currcy, currencies[i].name) == 0) {
			valid_currcy = true;
			btcdbg("valid currency");
			strcpy(btcrates.currcy.name, currencies[i].name);
			wcscpy(btcrates.currcy.sign, currencies[i].sign);
			btcrates.currcy.sf = currencies[i].sf;
			break;
		}
	}

	if(!valid_currcy) {
		btcdbg("!valid_currcy");
		api_err -> err = true;
		strcpy(api_err -> errstr, "invalid currcy");
		return NULL;
	}

	btcdbg("url old: %s", api_url);
	for(
		uint_fast8_t i = API_URL_CURRCY_POS, j = 0;
		i < (API_URL_CURRCY_POS + 3);
		++i, ++j
	) api_url[i] = mod_currcy[j];
	btcdbg("url new: %s", api_url);

	curl_easy_setopt(handle, CURLOPT_URL, api_url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);  // sets the function to call
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, json);  // sets the data to be given to the function

	result = curl_easy_perform(handle);  // performs the request, stores result
	if(result != CURLE_OK) {
		api_err -> err = true;
		strcpy(api_err -> errstr, curl_easy_strerror(result));
		return NULL;
	}

	btcdbg("json {\n%s\n}", json);

	curl_easy_cleanup(handle);
	curl_global_cleanup();

	return json;
}

bool parse_json(const char *const json, btcerr_t *const api_err) {
	btcdbg("parse_json()");

	json_t *buy;
	json_t *data;
	json_error_t json_error;
	json_t *root;
	json_t *sell;

	root = json_loads(json, 0, &json_error);
	if(!root) {
		api_err -> err = true;
		strcpy(api_err -> errstr, json_error.text);
		return false;
	}

	data = json_object_get(root, "data");
	buy = json_object_get(data, "buy");
	sell = json_object_get(data, "sell");

	btcrates.result = (strcmp(json_string_value(json_object_get(root, "result")), "success")) ? false : true;

	// stores trade values as int and float
	btcrates.buy = atoi(json_string_value(json_object_get(buy, "value_int")));
	btcrates.buyf = ((double) btcrates.buy / btcrates.currcy.sf);
	btcrates.sell = atoi(json_string_value(json_object_get(sell, "value_int")));
	btcrates.sellf = ((double) btcrates.sell / btcrates.currcy.sf);

	json_decref(root);
	return true;
}

size_t write_data(
	const char *const buffer,
	const size_t size,
	const size_t nmemb,
	const void *const userdata
) {
	btcdbg("write_data()");
	strcpy((char *) userdata, buffer);
	return (size * nmemb);
}
