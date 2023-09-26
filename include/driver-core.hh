#ifndef driver_core_hh
#define driver_core_hh

#include <libusb-1.0/libusb.h>
#include <map>
#include <string>

using std::map;
using std::string;

static constexpr int USB_BUFF_SIZE = 4096;

struct driver {
	driver() {}
	virtual ~driver() {}

	virtual int probe() = 0;
	virtual int get_programmer_info() = 0;
	virtual int xfer_bdm_data(char *io_buff, int len) = 0;
	virtual int send_big_block(uint8_t *data, uint32_t dest_addr,
				   int size) = 0;
	virtual void send_reset(bool state) = 0;
	virtual void send_go() = 0;
	virtual void send_halt() = 0;

protected:
	libusb_device *dev;
	libusb_device_handle *handle;
	unsigned int endpoint_in;
	unsigned int endpoint_out;
	unsigned char ibuf[USB_BUFF_SIZE];
	unsigned char obuf[USB_BUFF_SIZE];
};

struct driver_core {
	driver_core();
	~driver_core();

	int init();
	int detect_usb_pod();

	driver *get_current_driver() { return drv; }

	template<typename T> driver *create_driver(libusb_device *device);
	typedef driver *(driver_core::*mf)(libusb_device *device);
private:
	libusb_context *ctx;
	driver *drv{};

	map<string, mf> md;

};

#endif /* driver_core_hh */
