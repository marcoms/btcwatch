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

#ifndef BTCAPI_H
#define BTCAPI_H

#include <stdbool.h>
#include <stdlib.h>

// struct containing current exchange information
typedef struct rates {
	float buy;
	bool result;
	float sell;
} rates_t;

extern bool got_mtgox_rates;
extern rates_t mtgox_rates;

float buy(char *const currency, const char *const prog_name);

// uses libcURL to access a Bitcoin API, calls write_data, then returns a JSON string
char *get_json(char *currency, const char *const prog_name);

// uses jansson to parse the JSON string and returns a rates_t containing exchange information
rates_t parse_json(const char *const json, const char *const prog_name);

bool ping(char *const prog_name);
float sell(char *const currency, const char *const prog_name);

// libcURL callback function that copies the buffer to a local string
size_t write_data(
	char *buffer,
	size_t size,
	size_t nmemb,
	void *userdata
);

#endif
