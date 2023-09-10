#ifndef elf_hh
#define elf_hh

#include <string>

using std::string;

struct elf
{
	elf() {}

	char *load_elf(const string &path);
	int load_program_headers(const char *offs, int entries);
};

#endif /* elf_hh */
