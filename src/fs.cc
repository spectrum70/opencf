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

#include "fs.hh"
#include "trace.hh"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstddef>

using namespace trace;

namespace fs {

char *load_file_to_mem(const char *path)
{
	char *rval;
	struct stat st;
	int f, rd;

	if (stat(path, &st) != 0) {
		log_err("load: %s not found", path);
		return NULL;
	}

	f = open(path, 'r');
	if (f == -1) {
		log_err("error opening file");
		return NULL;
	}

	rval = new char[st.st_size];
	if (!rval) {
		log_err("cannot allocate memory");
		return NULL;
	}

	log_dbg("%s() reading: %d", __func__, st.st_size);

	rd = read(f, rval, st.st_size);
	if (rd != st.st_size) {
		log_err("error loading file to memory, rd = %d", rd);
		delete[] rval;
		return NULL;
	}

	return rval;
}

}
