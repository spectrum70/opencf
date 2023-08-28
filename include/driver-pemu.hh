#ifndef driver_pemu_hh
#define driver_pemu_hh

#include "driver-core.hh"
#include "coldfire.hh"

#include <libusb-1.0/libusb.h>
#include <cstdint>

struct driver_pemu : public driver {

	driver_pemu(libusb_device *device) { dev = device; }

	virtual int probe();
	virtual int get_programmer_info();
	virtual int check_connected_cpu();

private:
	int extract_info(unsigned char *offset, int pos, char *res);
	int send_and_recv(int tx_count, int rx_count);
	int send_generic(uint8_t cmd_type, uint16_t len);
	uint32_t bdm_read_ad_reg(uint8_t reg);
};

#endif /* driver_pemu_hh */
