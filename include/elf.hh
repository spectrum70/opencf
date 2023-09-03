#ifndef elf_hh
#define elf_hh

#include <string>

using std::string;

struct elf
{
	elf() {}

	char *load_elf(const string &path);
};

#endif /* elf_hh */
