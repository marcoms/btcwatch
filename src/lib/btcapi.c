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
#include "../include/btcdbg.h"
#include "../include/btcerr.h"

rates_t btcrates = {
	.got = false
};

int fill_rates(const char *const currcy, btcerr_t *const api_err) {
	btcdbg(__FILE__, __LINE__, "fill_rates()");

	char *json = get_json(currcy, api_err);
	if(api_err -> err) {
		return 0;
	}

	if(!parse_json(json, api_err)) {
		free(json);
		return 0;
	}

	free(json);
	return 1;
}

char *get_json(const char *const currcy, btcerr_t *const api_err) {
	
	btcdbg(__FILE__, __LINE__, "get_json()");
	

	char api_url[] = "https://data.mtgox.com/api/2/BTCxxx/money/ticker_fast";
	currcy_t currencies[] = {
		{
			// australia

			.name = "AUD",
			.sign = L"$",
		},

		{
			// canada

			.name = "CAD",
			.sign = L"$",
		},

		{
			// switzerland

			.name = "CHF",
			.sign = L"Fr."
		},

		{
			// china

			.name = "CNY",
			.sign = L"¥"
		},

		{
			// czech republic

			.name = "CZK",
			.sign = L"Kč."
		},

		{
			// denmark

			.name = "DKK",
			.sign = L"kr."
		},

		{
			// italy

			.name = "EUR",
			.sign = L"€"
		},

		{
			// great britain

			.name = "GBP",
			.sign = L"£"
		},

		{
			// hong kong

			.name = "HKD",
			.sign = L"$"
		},

		{
			// japan

			.name = "JPY",
			.sign = L"¥"
		},

		{
			// norway

			.name = "NOK",
			.sign = L"kr."
		},

		{
			// poland

			.name = "PLN",
			.sign = L"zł."
		},

		{
			// russia

			.name = "RUB",
			.sign = L"p."
		},

		{
			// sweden

			.name = "SEK",
			.sign = L"kr."
		},

		{
			// singapore

			.name = "SGD",
			.sign = L"$"
		},

		{
			// thailand

			.name = "THB",
			.sign = L"฿"
		},

		{
			// united states

			.name = "USD",
			.sign = L"$"
		},
	};

	char mod_currcy[3 + 1];
	strcpy(mod_currcy, currcy);

	char *json;
	CURL *handle;
	CURLcode result;
	bool valid_currcy;

	json = malloc(sizeof (char) * 1600);
	if(json == NULL) {
		btcdbg(__FILE__, __LINE__, "can't allocate memory");
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

	
	btcdbg(__FILE__, __LINE__, "currcy \"%s\" strlen: %d", currcy, strlen(currcy));
	

	// length check

	if(strlen(currcy) != 3) {
		btcdbg(__FILE__, __LINE__, "bad currency length");

		api_err -> err = true;
		strcpy(api_err -> errstr, "bad currency length");

		return NULL;
	}

	// case correction

	for(
		uint_fast8_t i = 0;
		i < (sizeof mod_currcy[0] * sizeof mod_currcy);
		++i
	) mod_currcy[i] = toupper(mod_currcy[i]);

	btcdbg(__FILE__, __LINE__, "mod_currcy \"%s\" strlen: %d", mod_currcy, strlen(mod_currcy));

	// validation

	for(
		uint_fast8_t i = 0;
		i < ((sizeof currencies / sizeof currencies[0]));
		++i
	) {
		if(strcmp(mod_currcy, currencies[i].name) == 0) {
			valid_currcy = true;

			btcdbg(__FILE__, __LINE__, "valid currency");

			strcpy(btcrates.currcy.name, currencies[i].name);
			wcscpy(btcrates.currcy.sign, currencies[i].sign);

			break;
		}
	}

	if(!valid_currcy) {

		btcdbg(__FILE__, __LINE__, "!valid_currcy");


		api_err -> err = true;
		strcpy(api_err -> errstr, "invalid currcy");

		return NULL;
	}

	btcdbg(__FILE__, __LINE__, "url old: %s", api_url);

	for(
		uint_fast8_t i = API_URL_CURRCY_POS, j = 0;
		i < (API_URL_CURRCY_POS + 3);
		++i, ++j
	) api_url[i] = mod_currcy[j];

	btcdbg(__FILE__, __LINE__, "url new: %s", api_url);

	curl_easy_setopt(handle, CURLOPT_URL, api_url);

	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);  // sets the function to call

	curl_easy_setopt(handle, CURLOPT_WRITEDATA, json);  // sets the data to be given to the function

	result = curl_easy_perform(handle);  // performs the request, stores result
	if(result != CURLE_OK) {
		api_err -> err = true;
		strcpy(api_err -> errstr, curl_easy_strerror(result));

		return NULL;
	}

	
	btcdbg(__FILE__, __LINE__, "json {\n%s\n}", json);
	

	curl_easy_cleanup(handle);
	curl_global_cleanup();

	return json;
}

int parse_json(const char *const json, btcerr_t *const api_err) {
	btcdbg(__FILE__, __LINE__, "parse_json()");

	json_t *buy;
	json_t *data;
	json_error_t json_error;
	json_t *root;
	json_t *sell;

	root = json_loads(json, 0, &json_error);
	if(!root) {
		strcpy(api_err -> errstr, json_error.text);
		return 0;
	}

	data = json_object_get(root, "data");
	buy = json_object_get(data, "buy");
	sell = json_object_get(data, "sell");

	btcrates.result = strcmp(json_string_value(json_object_get(root, "result")), "success") ? false : true;

	// stores trade values as float
	btcrates.buy = atof(json_string_value(json_object_get(buy, "value")));
	btcrates.sell = atof(json_string_value(json_object_get(sell, "value")));

	btcdbg(__FILE__, __LINE__, "buy %f", btcrates.buy);
	btcdbg(__FILE__, __LINE__, "sell %f", btcrates.sell);

	json_decref(root);

	return 1;
}

size_t write_data(
	char *buffer,
	size_t size,
	size_t nmemb,
	void *userdata
) {
	
	btcdbg(__FILE__, __LINE__, "write_data()");
	

	strcpy(userdata, buffer);

	return (size * nmemb);
}
