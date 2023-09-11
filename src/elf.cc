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

/*
 * cf64k.elf
 * Program Headers:
 *  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
 *  LOAD           0x0000f4 0x80000000 0x80000000 0x00670 0x00670 R E 0x4
 *  LOAD           0x000000 0x80001868 0x80000654 0x00000 0x0011b RW  0x4
 *  LOAD           0x000764 0x80000670 0x80000670 0x00144 0x00144 R E 0x2
 *  LOAD           0x000000 0x80001983 0x8000076f 0x00000 0x0007c RW  0x1
 *  LOAD           0x0008a8 0x800007b4 0x800007b4 0x010b2 0x010b2 R E 0x2
 *  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x10
 *
 * Section to Segment mapping:
 *  Segment Sections...
 *   00     .text .text.dcache_enable
 *   01     .bss
 */

int elf::load_program_headers(const char *offs, int entries)
{
	char *ptr = (char *)offs;

	log_dbg("%s() Offset   FileSiz", __func__);

	while (entries--) {
		Elf32_Phdr *phdr = (Elf32_Phdr *)ptr;

		int type = ntohl(phdr->p_type);
		int flags = ntohl(phdr->p_flags);

		if (type == PT_LOAD && flags == (PF_R | PF_X)) {
			log_dbg("%s() %08x %04x", __func__,
				ntohl(phdr->p_offset),
				ntohl(phdr->p_filesz));

			//bdm->load_segment(phdr->p_offset, phdr->p_filesz);
		}

		ptr += sizeof(Elf32_Phdr);
	}

	return 0;
}

char *elf::load_elf(const string &path)
{
	Elf32_Ehdr *ehdr;

	char *elf;
	unsigned char *p;
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

	if (ehdr->e_phoff)
		load_program_headers(&elf[ntohl(ehdr->e_phoff)],
				     ntohs(ehdr->e_phnum));

	return elf;

exit_err:
	delete[] elf;

	return NULL;
}
