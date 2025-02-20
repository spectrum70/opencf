/*
 * opencf - a ColdFire CPU family programming tool
 *
 * Copyright 2023 Angelo Dureghello
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "trace.hh"
#include "getopts.hh"

#include <iostream>
#include <cstdio>

#define ANSI_COLOR_IMPORTANT	"\x1b[1;35m"
#define ANSI_COLOR_ERROR	"\x1b[1;31m"

using namespace std;

namespace trace {

void log(const char *color, const char *format, va_list args)
{
	printf(color);
	vprintf(format, args);
	printf(ANSI_COLOR_RESET "\n");
}

void log_ansi(const char *code, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	log(code, format, args);
	va_end(args);
}

void log_info(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	log(ANSI_COLOR_YELLOW, format, args);
	va_end(args);
}

void log_imp(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	log(ANSI_COLOR_IMPORTANT, format, args);
	va_end(args);
}

void log_wrn(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	log(ANSI_COLOR_MAGENTA, format, args);
	va_end(args);
}

void log_dbg(const char *format, ...)
{
	va_list args;

	if (opts::get().verbose) {
		va_start(args, format);
		log(ANSI_COLOR_RESET, format, args);
		va_end(args);
	}
}

void log_err(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	log(ANSI_COLOR_RED, format, args);
	va_end(args);
}

void log_buffer(const unsigned char *data, int size)
{
	int i = 0;

	for (; i < size; ++i) {
		if (!(i%16)) {
			if (i)
				printf("\n");
			printf(ANSI_COLOR_YELLOW);
			printf("%08x:", i);
			printf(ANSI_COLOR_CYAN);
		}
		printf("%02x ", (int)data[i]);
	}
	printf("\n");
	printf(ANSI_COLOR_RESET);
}

};
