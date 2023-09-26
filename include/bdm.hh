#ifndef bdm_hh
#define bdm_hh

#include "coldfire.hh"
#include "bdm-defs.hh"
#include <cstdint>

class driver;

static constexpr int max_bdm_buff = 2048;

/*
 * BDM registers
 */
enum bdm_cf26_registers {
	BDM_REG_CSR = 0x00,
	BDM_REG_XCSR = 0x01,
};

constexpr int CSR_SSM = (1 << 4);
constexpr int CSR_IPI = (1 << 5);
constexpr int CSR_NPL = (1 << 6);
constexpr int CSR_EMULATION = (1 << 14);
constexpr int CSR_MAP = (1 << 15);
constexpr int CSR_BPKT = (1 << 24);
constexpr int CSR_HALT = (1 << 25);
constexpr int CSR_TRG = (1 << 26);

/* Control reg types */
enum  cr_type {
	crt_cacr = 0x002,
	crt_acr0 = 0x004,
	crt_acr1 = 0x005,
	crt_vbr = 0x801,
	crt_macsr = 0x804,
	crt_mask = 0x805,
	crt_acc = 0x806,
	crt_sr = 0x80e,
	crt_pc = 0x80f,
	crt_rambar = 0xc04,
};

enum states {
	st_halted,
	st_step,
	st_running,
};

class bdm_ops
{
public:
	bdm_ops(driver *current_driver);

	void reset(bool state);
	void go();
	void halt();
	uint32_t step();
	uint32_t read_dm_reg(uint8_t reg);
	uint32_t write_dm_reg(uint8_t reg, uint32_t value);
	uint32_t read_ad_reg(uint8_t reg);
	uint32_t write_ad_reg(uint8_t reg, uint32_t value);
	uint32_t read_mem_byte(uint32_t address);
	uint32_t read_mem_word(uint32_t address);
	uint32_t read_mem_long(uint32_t address);
	uint32_t write_mem_byte(uint32_t address, uint8_t value);
	uint32_t write_mem_word(uint32_t address, uint16_t value);
	uint32_t write_mem_long(uint32_t address, uint32_t value);
	uint32_t read_ctrl_reg(cr_type type);
	uint32_t write_ctrl_reg(cr_type type, uint32_t value);
	int load_segment(uint8_t *data, uint32_t dest, uint32_t size);

private:
	int state {};
	driver *drv;
	char buff[max_bdm_buff];
};


#endif /* bdm_hh */
