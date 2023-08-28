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

#include "driver-pemu.hh"
#include "trace.hh"
#include <libusb-1.0/libusb.h>

using namespace trace;

int driver_pemu::send_and_recv()
{
	unsigned int count;
	int transferred;

	/* send request */
	//count = jtag_libusb_bulk_write(pemu->devh,
	//			       pemu->endpoint_out | LIBUSB_ENDPOINT_OUT,
	//				(char *)pemu->buffer, pemu->count, PEMU_USB_TIMEOUT);

	libusb_bulk_transfer(handle,
			     endpoint_out | LIBUSB_ENDPOINT_OUT,
			     obuf, count, &transferred, 0);


	/*if (count != pemu->count) {
		+		LOG_ERROR("PEMU communication error: can't write");
		+		return ERROR_FAIL;
		+	}
		+
		+	count = jtag_libusb_bulk_read(pemu->devh,
						      +			pemu->endpoint_in | LIBUSB_ENDPOINT_IN ,
				  +			(char *)pemu->buffer,
						      +			PEMU_STD_PKT_SIZE, PEMU_USB_TIMEOUT * 3);
		+
		+	if (count != PEMU_STD_PKT_SIZE) {
			+		LOG_ERROR("PEMU communication error: can't read");
			+		return ERROR_FAIL;
			+	}
			+
			+	return ERROR_OK;
			+}*/

}

int driver_pemu::get_version()
{
	/*char version_fw[16];

	h_u16_to_be(&bdev->buffer[0], PEMU_PT_GENERIC);
	h_u16_to_be(&bdev->buffer[2], 2);
	bdev->buffer[4] = CMD_TYPE_GENERIC;
	bdev->buffer[5] = CMD_PEMU_GET_VERSION_STR;

	bdev->count = PEMU_STD_PKT_SIZE;

+	if (pemu_send_and_recv(bdev) != ERROR_OK)
+		return ERROR_FAIL;
+
+	if (memcmp(bdev->buffer,
			   +		REP_VERSION_INFO, sizeof(REP_VERSION_INFO)) != 0) {
		+		LOG_ERROR("PEMU communication error: can't get version msg");
		+		return ERROR_FAIL;
		+	}
		+
		+	if (get_version_field(&bdev->buffer[PEMU_CMD_REPLY_LEN],
					      +		7, version_fw) != ERROR_OK) {
			+		LOG_ERROR("PEMU communication error: can't get version info");
			+		return ERROR_FAIL;
			+	}
			+
			+	LOG_INFO("P&E Multilink Universal fw version %s", version_fw);
			+
			+	if (strcmp(version_fw, "9.60") != 0) {
				+		LOG_ERROR("P&E Multilink Universal wrong FW version, "
				+			  "please flash correct CFv234 firmware."
				+		);
				+		return ERROR_FAIL;
				+	}
				+
				+	return ERROR_OK;
				+}
*/
}

int driver_pemu::probe()
{
	log_info("%s() entering", __func__);

	libusb_open(dev, &handle);

	return 0;
}


