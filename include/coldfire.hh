#ifndef coldfire_hh
#define coldfire_hh

#include <cstdint>

struct inf_d0 {
	uint32_t dbg_rev:4;
	uint32_t isa_rev:4;
	uint32_t l2cc:1;
	uint32_t res:2;
	uint32_t mmu:1;
	uint32_t fpu:1;
	uint32_t emac:1;
	uint32_t div:1;
	uint32_t mac:1;
	uint32_t revision:4;
	uint32_t version:4;
	uint32_t magic:8;

};

struct inf_d1 {
	uint32_t sz_rom1:4;
	uint32_t sz_sram1:4;
	uint32_t sz_dcache:4;
	uint32_t dcache_ass:2;
	uint32_t sz_mbus:2;
	uint32_t sz_rom0:4;
	uint32_t sz_sram0:4;
	uint32_t sz_icache:4;
	uint32_t icache_ass:2;
	uint32_t sz_chache_line:2;
};

struct cpu_info {
	union {
		uint32_t reg;
		struct inf_d0 f;
	} d0;
	union {
		uint32_t reg;
		struct inf_d1 f;
	} d1;
};

enum {
	CF_D0,
	CF_D1,
	CF_D2,
	CF_D3,
	CF_D4,
	CF_D5,
	CF_D6,
	CF_D7,
	CF_A0,
	CF_A1,
	CF_A2,
	CF_A3,
	CF_A4,
	CF_A5,
	CF_FP,
	CF_SP,
	CF_PS,
	CF_PC,
	CF_VBR,
	CF_NUM_REGS,
};

#endif /* coldfire_hh */
