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

#include "utils.hh"

#include <iomanip>
#include <sstream>

namespace utils {

uint16_t ntohs(uint16_t val)
{
	return (val << 8 | val >> 8);
}

uint32_t ntohl(uint32_t val)
{
	return (val << 24) |
	       ((val << 8) & 0x00ff0000) |
	       ((val >> 8) & 0x0000ff00) |
	       (val >> 24);
}

unsigned int str_to_bin(string &str)
{
	stringstream ss;
	unsigned int rval;

	ss << str;
	if (str[0] == '0' && str[1] == 'x')
		ss >> hex >> rval;
	else
		ss >> dec >> rval;

	return rval;
}

}
