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

class bdm_ops
{
public:
	bdm_ops(driver *current_driver);

	void reset(bool state);
	uint32_t read_ad_reg(uint8_t reg);
	uint32_t read_dm_reg(uint8_t reg);
	uint32_t write_ad_reg(uint8_t reg, uint32_t value);
	uint32_t read_mem_byte(uint32_t address);
	uint32_t write_mem_byte(uint32_t address, uint8_t value);
	int load_segment(uint8_t *data, uint32_t dest, uint32_t size);

private:
	driver *drv;
	char buff[max_bdm_buff];
};


#endif /* bdm_hh */
