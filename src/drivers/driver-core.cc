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
	string name;
};

static struct usb_ids ids[] = {
	{ 0x1357, 0x0503, "driver_pemu", "P&E Multilink Universal" },
	{ 0x16d0, 0x0567, "to-do", "USBDM HCS08,HCS12,Coldfire-V1 BDM" },
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
	int dev_count, i, n, rval = 0;

	dev_count = libusb_get_device_list(ctx, &list);

	for (i = 0; i < dev_count; i++) {
		struct libusb_device* device = list[i];
		struct libusb_device_descriptor desc;

		libusb_get_device_descriptor(device, &desc);

		for (n = 0 ;;) {
			if (ids[n].id_vendor == 0)
				break;

			if (desc.idVendor == ids[n].id_vendor &&
				desc.idProduct == ids[n].id_product) {

				log_info("programmer detected: %s",
					 ids[n].name.c_str());

				drv = (this->*md[ids[n].class_name])(device);

				goto exit_detect;
			}
			n++;
		}
	}

	rval = -1;

exit_detect:
	libusb_free_device_list(list, 1);

	return rval;
}

int driver_core::init()
{
	int err = 0;

	err = libusb_init(&ctx);
	if (err) {
		log_err("cannot initialize libusb, error %s", err);
		goto exit;
	}

	/* First stage is detecting usb pod  */
	err = detect_usb_pod();
	if (err) {
		log_err("no usb device found, exiting");
		goto exit;
	}

	err = drv->probe();
	if (err) {
		log_err("device probe failed, exiting");
		goto exit;
	}

	err = drv->get_programmer_info();
	if (err) {
		log_err("cannot read programmer info, exiting");
		goto exit;
	}

	err = drv->check_connected_cpu();
	if (err) {
		log_err("no cpu connected, exiting");
		goto exit;
	}


exit:
	libusb_exit(ctx);

	return err;
}


