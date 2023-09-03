#ifndef bdm_hh
#define bdm_hh

#include "coldfire.hh"
#include "bdm-defs.hh"
#include <cstdint>

class driver;

static constexpr int max_bdm_buff = 2048;

class bdm_ops
{
public:
	bdm_ops(driver *current_driver);

	void reset(bool state);
	uint32_t read_ad_reg(uint8_t reg);
private:
	driver *drv;
	char buff[max_bdm_buff];
};


#endif /* bdm_hh */
