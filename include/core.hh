#ifndef core_hh
#define core_hh

#include "driver-core.hh"
#include "bdm.hh"
#include <string>

using std::string;

struct cf_cpu {
	uint32_t d0_rst;
	uint32_t d1_rst;
	const char *name;
};

class core
{
public:
	core();
	~core();

	int run();

private:
	int examine();
	int get_programmer_info();
	int get_cpu_info();

private:
	driver_core dc;
	driver *drv;
	bdm_ops *bdm;
};


#endif /* core_hh */
