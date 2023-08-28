/*
 * opencf - a ColdFire CPU family programming tool
 *
 * Copyright 2023 Angelo Dureghello
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "driver-core.hh"
#include "driver-pemu.hh"
#include "trace.hh"

using namespace trace;

struct usb_ids {
	int id_vendor;
	int id_product;
	string class_name;
};

static struct usb_ids ids[] = {
	{ 0x1357, 0x0503, "driver_pemu" },
	0
};

template <typename T> driver *driver_core::create_driver(libusb_device *device)
{ return new T(device); }

driver_core::driver_core()
{
	md["driver_pemu"] = &driver_core::create_driver<driver_pemu>;
}

driver_core::~driver_core()
{
}

int driver_core::detect_usb_pod()
{
	libusb_device **list;
	int dev_count, i, n;

	dev_count = libusb_get_device_list(ctx, &list);

	for (i = 0; i < dev_count; i++) {
		struct libusb_device* device = list[i];
		struct libusb_device_descriptor desc;

		libusb_get_device_descriptor(device, &desc);

		log_dbg("%s() vendor:product %04x:%04x", __func__,
			desc.idVendor, desc.idProduct);

		for (n = 0 ;;) {
			if (ids[n].id_vendor == 0)
				break;

			if (desc.idVendor == ids[n].id_vendor &&
				desc.idProduct == ids[n].id_product) {
				log_info("hurra !!!");

				drv = (this->*md[ids[n].class_name])(device);

				return 0;
			}

			n++;
		}
	}

	log_err("no usb device found, exiting");

	libusb_free_device_list(list, 1);

	return -1;
}

int driver_core::init()
{
	int err;

	err = libusb_init(&ctx);
	if (err)
		log_err("cannot initialize libusb, error %s", err);

	/* First stage is detecting usb pod is possible */
	detect_usb_pod();

	drv->probe();

	libusb_exit(ctx);

	return 0;
}


