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

#include "../../config.h"

#if HAVE_LIBCURL
	#include <curl/curl.h>			// curl_easy_init, curl_easy_perform, curl_easy_setopt, CURLcode, CURLE_OK, CURLOPT_URL, CURLOPT_WRITEDATA, CURLOPT_WRITEFUNCTION,CURL
#else
	#error libcurl not found
#endif

#if HAVE_LIBJANSSON
	#include <jansson.h>			// json_error_t, json_loads, json_object_get, json_string_value,json_t
#else
	#error libjansson not found
#endif

#if HAVE_LIBC
	#include <stdbool.h>			// bool, false, true
#include <stdio.h>					// fprintf
	#include <string.h>				// atof, strcmp
#else
#error libc not found
#endif

#include "../include/btcapi.h"		// get_api, parse_json, rates_t
#include "../include/errutils.h"	// ERR

char *get_api(const char *const url, const char *const prog_name) {
	CURL *handle;
	char *json;
	CURLcode result;

	handle = curl_easy_init();
	json = malloc(sizeof (char) * 768);

	if(!handle) {
		ERR(prog_name, "unable to initialise libcurl session");

		return NULL;
	}

	if(!json) {
		ERR(prog_name, "unable to allocate memory");

		return NULL;
	}

	curl_easy_setopt(
		handle,
		CURLOPT_URL,
		url
	);

	curl_easy_setopt(
		handle,
		CURLOPT_WRITEFUNCTION,
		write_data
	);

	curl_easy_setopt(
		handle,
		CURLOPT_WRITEDATA,
		json
	);

	result = curl_easy_perform(handle);

	if(result != CURLE_OK) {
		fprintf(
			stderr,
			"%s: error: unable to perform cURL request (%d)\n",
			prog_name,
			result
		);

		return NULL;
	}

	return json;
}

rates_t parse_json(const char *const json, const char *const prog_name) {
	json_t *buy;
	json_t *data;
	json_error_t error;
	rates_t json_rates;
	json_t *root;
	json_t *sell;

	root = json_loads(
		json,
		0,
		&error
	);

	if(!root) fprintf(
		stderr,
		"%s: error: on line %d: %s\n",
		prog_name,
		error.line,
		error.text
	);

	json_rates.result = (
		strcmp(
			json_string_value(
				json_object_get(
					root,
					"result"
				)
			),
			"success"
		) ? false : true
	);

	data = json_object_get(root, "data");

	buy = json_object_get(data, "buy");

	sell = json_object_get(data, "sell");

	json_rates.buy = atof(
		json_string_value(
			json_object_get(
				buy,
				"value"
			)
		)
	);

	json_rates.sell = atof(
		json_string_value(
			json_object_get(
				sell,
				"value"
			)
		)
	);

	return json_rates;
}

size_t write_data(
	char *buffer,
	size_t size,
	size_t nmemb,
	void *userdata
) {
	strcpy(userdata, buffer);

	return (size * nmemb);
}
