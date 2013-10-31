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

#ifndef BTCAPI_H
#define BTCAPI_H

#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>

#include "btcerr.h"

typedef struct {
	char name[3 + 1];
	wchar_t sign[3 + 1];
	double sf;
} currcy_t;

// struct containing current exchange information

typedef struct {
	uint32_t buy;		// buy value as an integer (more precise)
	double buyf;		// buy value as a double-precision float
	currcy_t currcy;	// structure containing currency information
	bool got;		// obtained?
	bool result;		// result of the JSON string
	uint32_t sell;		// sell value as an integer (^)
	double sellf;		// sell value as a double-precision float
} rates_t;

extern rates_t btcrates;

// poplates the global btcrates struct

bool fill_rates(const char *const currcy, btcerr_t *const api_err);

// uses libcURL to access a Bitcoin API, calls write_data, then returns a JSON string

char *get_json(const char *currcy, btcerr_t *const api_err);


// uses jansson to parse the JSON string and returns a rates_t containing exchange information

bool parse_json(const char *const json, btcerr_t *const api_err);

// libcURL callback function that copies the buffer to a local string

size_t write_data(
	const char *const buffer,
	const size_t size,
	const size_t nmemb,
	const void *const userdata
);

#endif
