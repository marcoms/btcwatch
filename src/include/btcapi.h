#ifndef BTCAPI_H
#define BTCAPI_H

#define BTCAPI_PRINT_ERR(prog_name, err) fprintf(stderr, "%s: error: %s\n", prog_name, err)

#include <stdbool.h>	// bool
#include <stdlib.h>		// size_t

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

// struct containing current exchange information
typedef struct rates {
	bool result;
	float buy;
	float sell;
} rates_t;

// uses libcURL to access a Bitcoin API, calls write_data, then returns a JSON string
char* get_api(const char *const url, const char *const prog_name);

// uses jansson to parse the JSON string and returns a rates_t containing exchange information
rates_t parse_json(const char *const json, const char *const prog_name);

// libcURL callback function that copies the buffer to a local string
size_t write_data(
	char *buffer,
	size_t size,
	size_t nmemb,
	void *userdata
);

#endif
