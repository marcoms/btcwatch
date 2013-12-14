/*
	Copyright (C) 2013  Marco Scannadinari

	This file is part of btcwatch.

	btcwatch is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	btcwatch is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with btcwatch.  If not, see <http://www.gnu.org/licenses/>.
*/

#if MT_GOX_API
#define API_URL_CURRCY_POS 32  // position to insert currency
#elif BTC_E_API
#define API_URL_CURRCY_POS  28  // ^
#endif

#include <curl/curl.h>
#include <ctype.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "btcapi.h"

rates_t btcrates = {
	.got = false
};

bool fill_rates(const char *const currcy, btcerr_t *const api_err) {
	char *json;

	json = get_json(currcy, api_err);
	if(api_err -> err) {
		free(json);
		return false;
	}

	parse_json(json, api_err);
	free(json);

	if(api_err -> err) return false;
	return true;
}

char *get_json(const char *const currcy, btcerr_t *const api_err) {
	#ifdef MT_GOX_API
	char api_url[] = "https://data.mtgox.com/api/2/BTCxxx/money/ticker_fast";
	#elif defined(BTC_E_API)
	char api_url[] = "https://btc-e.com/api/2/btc_xxx/ticker";
	#endif

	currcy_t currencies[] = {
		#ifdef MT_GOX_API
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
		#endif

		// eurozone

		{
			.name = "EUR",
			.sign = L"€",
			.sf = (1e5)
		},

		#ifdef MT_GOX_API
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
		#endif

		// russia

		{
			#ifdef MT_GOX_API
			.name = "RUB",
			#elif defined(BTC_E_API)
			.name = "RUR",
			#endif
			.sign = L"p.",
			.sf = (1e5)
		},

		#ifdef MT_GOX_API
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
		#endif

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


	// length check

	if(strlen(currcy) != 3) {
		api_err -> err = true;
		strcpy(api_err -> errstr, "bad currency length");
		return NULL;
	}

	// uppercases the currency string

	for(
		uint_fast8_t i = 0;
		i < ((sizeof mod_currcy[0]) * (sizeof mod_currcy));
		++i
	) mod_currcy[i] = toupper(mod_currcy[i]);

	// validation

	for(
		uint_fast8_t i = 0;
		i < ((sizeof currencies / sizeof currencies[0]));
		++i
	) {
		if(strcmp(mod_currcy, currencies[i].name) == 0) {
			valid_currcy = true;
			strcpy(btcrates.currcy.name, currencies[i].name);
			wcscpy(btcrates.currcy.sign, currencies[i].sign);
			btcrates.currcy.sf = currencies[i].sf;
			break;
		}
	}

	if(!valid_currcy) {
		api_err -> err = true;
		strcpy(api_err -> errstr, "invalid currcy");
		return NULL;
	}

	#ifdef BTC_E_API
	// lowercases the currency string

	for(
		uint_fast8_t i = 0;
		i < ((sizeof mod_currcy[0]) * (sizeof mod_currcy));
		++i
	) mod_currcy[i] = tolower(mod_currcy[i]);
	#endif

	for(
		uint_fast8_t i = API_URL_CURRCY_POS, j = 0;
		i < (API_URL_CURRCY_POS + 3);
		++i, ++j
	) api_url[i] = mod_currcy[j];

	curl_easy_setopt(handle, CURLOPT_URL, api_url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);  // sets the function to call
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, json);  // sets the data to be given to the function

	result = curl_easy_perform(handle);  // performs the request, stores result
	if(result != CURLE_OK) {
		api_err -> err = true;
		strcpy(api_err -> errstr, curl_easy_strerror(result));
		return NULL;
	}

	curl_easy_cleanup(handle);
	curl_global_cleanup();

	return json;
}

bool parse_json(const char *const json, btcerr_t *const api_err) {
	#ifdef MT_GOX_API
	json_t *buy;
	json_t *data;
	#endif
	json_error_t json_error;
	json_t *root;
	#ifdef MT_GOX_API
	json_t *sell;
	#elif defined(BTC_E_API)
	json_t *ticker;
	#endif

	root = json_loads(json, 0, &json_error);
	if(!root) {
		api_err -> err = true;
		strcpy(api_err -> errstr, json_error.text);
		return false;
	}

	#ifdef MT_GOX_API
	data = json_object_get(root, "data");
	if(!data) {
		api_err -> err = true;
		strcpy(api_err -> errstr, "couldn't get JSON object");
		return false;
	}
	#elif defined(BTC_E_API)
	ticker = json_object_get(root, "ticker");
	if(!ticker) {
		api_err -> err = true;
		strcpy(api_err -> errstr, "couldn't get JSON object");
		return false;
	}
	#endif

	#ifdef MT_GOX_API
	buy = json_object_get(data, "buy");
	sell = json_object_get(data, "sell");
	if(!buy || !sell) {
		api_err -> err = true;
		strcpy(api_err -> errstr, "couldn't get JSON object");
		return false;
	}

	btcrates.result = (strcmp(json_string_value(json_object_get(root, "result")), "success")) ? false : true;
	#endif

	// stores trade values as int and float

	#ifdef MT_GOX_API
	btcrates.buy = atof(json_string_value(json_object_get(buy, "value_int")));  // MtGox uses string for their numbers for whatever reason
	btcrates.buyf = ((double) btcrates.buy / btcrates.currcy.sf);
	btcrates.sell = atof(json_string_value(json_object_get(sell, "value_int")));
	btcrates.sellf = ((double) btcrates.sell / btcrates.currcy.sf);
	#elif defined(BTC_E_API)
	btcrates.buyf = (double) json_number_value(json_object_get(ticker, "buy"));  // no integer value in BTC-e's API so have to calculate it manually
	btcrates.buy = (int) (btcrates.buyf * btcrates.currcy.sf);
	btcrates.sellf = (double) json_number_value(json_object_get(ticker, "sell"));
	btcrates.sell = (int) (btcrates.sellf * btcrates.currcy.sf);
	#endif

	json_decref(root);
	return true;
}

size_t write_data(
	const char *const buffer,
	const size_t size,
	const size_t nmemb,
	const void *const userdata
) {
	strcpy((char *) userdata, buffer);
	return (size * nmemb);
}
