#ifndef driver_pemu_hh
#define driver_pemu_hh

#include "driver-core.hh"
#include "coldfire.hh"

#include <libusb-1.0/libusb.h>
#include <cstdint>
#include <map>
#include <tuple>

using std::map;
using std::tuple;

struct driver_pemu : public driver {

	driver_pemu(libusb_device *device);

	virtual int probe();
	virtual int get_programmer_info();
	virtual int xfer_bdm_data(char *io_buff, int size);
	virtual void send_reset(bool state);

private:
	int extract_info(unsigned char *offset, int pos, char *res);
	int send_and_recv(int tx_count, int rx_count);
	int send_generic(uint8_t cmd_type, uint16_t len);


	map<int, tuple<int, int>> bdm_prefixes;
};

#endif /* driver_pemu_hh */
