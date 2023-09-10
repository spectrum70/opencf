#ifndef bdm_defs_hh
#define bdm_defs_hh

enum bdmcf_commands {
	CMD_BDMCF_CMD_NOP = 0x0000,
	CMD_BDMCF_GO = 0x0c00,
	CMD_BDMCF_RDMREG = 0x2d80,
	CMD_BDMCF_WDMREG = 0x2c80,
	CMD_BDMCF_RCREG = 0x2980,
	CMD_BDMCF_WCREG = 0x2880,
	CMD_BDMCF_RDAREG = 0x2180,
	CMD_BDMCF_WDAREG = 0x2080,
};

#endif /* bdm_defs_hh */
