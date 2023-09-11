#ifndef elf_hh
#define elf_hh

#include "bdm.hh"
#include <string>

using std::string;

struct elf
{
	elf(bdm_ops *b) : bdm(b) {}

	char *load_elf(const string &path);
	int load_program_headers(const char *offs, int entries);

private:
	bdm_ops *bdm;
};

#endif /* elf_hh */
