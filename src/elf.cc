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
#include "fs.hh"
#include "utils.hh"
#include "elf.hh"

#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <elf.h>

using namespace trace;

static constexpr uint32_t elf_magic = 0x464c457f;

using namespace fs;
using namespace utils;

char *elf::load_elf(const string &path)
{
	Elf32_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	char *elf;
	uint16_t machine;

	log_dbg("%s() loading: %s", __func__, path.c_str());

	elf = load_file_to_mem(path.c_str());
	if (!elf)
		return NULL;

	ehdr = (Elf32_Ehdr *)elf;

	if (strncmp((char *)ehdr->e_ident, ELFMAG, 4) != 0) {
		log_err("not an elf file");
		goto exit_err;
	}

	machine = ntohs(ehdr->e_machine);

	if (machine != EM_COLDFIRE && machine != EM_68K) {
		log_err("invalid elf architecture: %d", ehdr->e_machine);
		goto exit_err;
	}

	log_dbg("%s() e_entry %08x, e_phoff %08x", __func__,
					ntohl(ehdr->e_entry),
					ntohl(ehdr->e_phoff));

	phdr = (Elf64_Phdr *)&elf[ntohl(ehdr->e_phoff)];

	log_dbg("%s() %08x", __func__, phdr->p_offset);

	return elf;

exit_err:
	delete[] elf;

	return NULL;
}
