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
#include "utils.hh"
#include "trace.hh"
#include "bdm-defs.hh"

#include <cstring>
#include <libusb-1.0/libusb.h>

using namespace trace;
using namespace utils;

static constexpr unsigned char REP_VERSION_INFO[] = {0x99, 0x66, 0x00, 0x64};

static constexpr int OFS_BDM = 5;
static constexpr int PEMU_CMD_REPLY_LEN = 4;
static constexpr int PEMU_STD_PKT_SIZE = 256;
static constexpr int PEMU_MAX_PKT_SIZE = 1280;
static constexpr int PEMU_MAX_BIG_BLOCK	= 0x4a8;

enum pemu_prefixes {
	CMD_PEMU_RESET = 0x01,
	CMD_PEMU_GO = 0x02,
	CMD_PEMU_GET_VERSION_STR = 0x0b,
	CMD_PEMU_BDM_MEM_R = 0x11,
	CMD_PEMU_BDM_REG_R = 0x13,
	CMD_PEMU_BDM_SCR_W = 0x14,
	CMD_PEMU_BDM_MEM_W = 0x15,
	CMD_PEMU_BDM_REG_W = 0x16,
	CMD_PEMU_GET_ALL_CPU_REGS = 0x18,
	CMD_PEMU_W_MEM_BLOCK = 0x19,
};

enum pemu_pkt_types {
	PEMU_PT_GENERIC = 0xaa55,
	PEMU_PT_CMD = 0xab56,
	PEMU_PT_WBLOCK = 0xbc67,
};

enum pemu_cmd_type {
	CMD_TYPE_GENERIC = 0x01,
	CMD_TYPE_IFACE = 0x04,
	CMD_TYPE_DATA = 0x07,
};

/*
 * pemu sends pre-commands based on bdm command to be sent
 */
driver_pemu::driver_pemu(libusb_device *device)
{
	dev = device;

	bdm_prefixes[CMD_BDMCF_RDMREG] =
		tuple(CMD_PEMU_BDM_REG_R, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_WDMREG] =
		tuple(CMD_PEMU_BDM_REG_W, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_RDAREG] =
		tuple(CMD_PEMU_BDM_REG_R, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_WDAREG] =
		tuple(CMD_PEMU_BDM_REG_W, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_RD_MEM_B] =
		tuple(CMD_PEMU_BDM_MEM_R, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_RD_MEM_W] =
		tuple(CMD_PEMU_BDM_MEM_R, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_RD_MEM_L] =
		tuple(CMD_PEMU_BDM_MEM_R, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_WR_MEM_B] =
		tuple(CMD_PEMU_BDM_MEM_W, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_WR_MEM_W] =
		tuple(CMD_PEMU_BDM_MEM_W, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_WR_MEM_L] =
		tuple(CMD_PEMU_BDM_MEM_W, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_RCREG] =
		tuple(CMD_PEMU_BDM_MEM_R, CMD_TYPE_DATA);
	bdm_prefixes[CMD_BDMCF_WCREG] =
		tuple(CMD_PEMU_BDM_SCR_W, CMD_TYPE_DATA);
}

int driver_pemu::send_and_recv(int tx_count, int rx_count)
{
	int rval;
	int transferred;

	rval = libusb_bulk_transfer(handle, endpoint_out | LIBUSB_ENDPOINT_OUT,
				    obuf, tx_count, &transferred, 0);
	if (rval || transferred != tx_count) {
		log_err("pemu communication error: can't write, %d %d",
			rval, tx_count);
		return 1;
	}

	rval = libusb_bulk_transfer(handle, endpoint_in | LIBUSB_ENDPOINT_IN,
				    ibuf, rx_count, &transferred, 0);
	if (rval || transferred != rx_count) {
		log_err("pemu communication error: can't read");
		return 1;
	}

	return 0;
}

int driver_pemu::extract_info(unsigned char *offset, int pos, char *res)
{
	int p = 0;
	char *r;

	r = strtok((char *)offset, ",");
	for (pos--; p < pos; p++) {
		r = strtok(0, ",");
	}

	strcpy(res, r);

	return 0;
}

int driver_pemu::send_generic(uint8_t cmd_type, uint16_t len)
{
	*(uint16_t *)&obuf[0] = ntohs(PEMU_PT_CMD);
	*(uint16_t *)&obuf[2] = ntohs(len + 1);
	obuf[4] = cmd_type;

	if (send_and_recv(PEMU_STD_PKT_SIZE, PEMU_STD_PKT_SIZE) != 0)
		return 1;

	return 0;
}

int driver_pemu::xfer_bdm_data(char *io_buff, int size)
{
	int midx = ntohs(*(uint16_t *)io_buff) & 0xfff0;

	if (bdm_prefixes.find(midx) == bdm_prefixes.end()) {
		log_err("bdm prefix not found");
		return 1;
	}

	obuf[OFS_BDM] = std::get<0>(bdm_prefixes[midx]);

	memcpy(&obuf[OFS_BDM + 1], io_buff, size);
	send_generic(std::get<1>(bdm_prefixes[midx]), size);
	memcpy(io_buff, &ibuf[OFS_BDM], PEMU_STD_PKT_SIZE - OFS_BDM);

	return 0;
}

int driver_pemu::send_big_block(uint8_t *data, uint32_t dest_addr, int size)
{
	uint16_t to_send, remainder;

	remainder = size % 4;

	while (size >= 4) {
		if (size > PEMU_MAX_BIG_BLOCK)
			to_send = PEMU_MAX_BIG_BLOCK;
		else
			to_send = size - remainder;

		*(uint16_t *)&obuf[0] = ntohs(PEMU_PT_WBLOCK);
		obuf[4] = CMD_TYPE_DATA;
		obuf[5] = CMD_PEMU_W_MEM_BLOCK;
		*(uint16_t *)&obuf[2] = ntohs(to_send + 8);
		*(uint16_t *)&obuf[6] = ntohs(to_send);
		*(uint32_t *)&obuf[8] = ntohl(dest_addr);

		memcpy(&obuf[12], data, to_send);

		/* pemu wants a padded packet */
		send_and_recv(PEMU_MAX_PKT_SIZE, PEMU_STD_PKT_SIZE);

		size -= to_send;
		data += to_send;
		dest_addr += to_send;
	}

	//while (remainder--)
		//pemu_write_mem_byte(dest_addr++, *data++);

	return 0;
}

void driver_pemu::send_reset(bool state)
{
	obuf[OFS_BDM] = CMD_PEMU_RESET;
	obuf[OFS_BDM + 1] = state ? 0xf0 : 0xf8;

	send_generic(CMD_TYPE_DATA, 2);
}

void driver_pemu::send_halt()
{
	send_reset(false);
}

void driver_pemu::send_go()
{
	obuf[OFS_BDM] = CMD_PEMU_GO;
	obuf[OFS_BDM + 1] = 0xfc;
	*(uint16_t *)&obuf[OFS_BDM + 2] = ntohs(CMD_BDMCF_GO);

	send_generic(CMD_TYPE_DATA, 4);

	obuf[OFS_BDM] = CMD_PEMU_BDM_REG_R;
	*(uint16_t *)&obuf[OFS_BDM + 1] = ntohs(CMD_BDMCF_RDMREG);

	send_generic(CMD_TYPE_DATA, 3);
}

int driver_pemu::get_programmer_info()
{
	int err;
	char version_fw[16];

	log_dbg("%s() entering", __func__);

	memset(obuf, 0, PEMU_STD_PKT_SIZE);

	*(uint16_t *)&obuf[0] = ntohs(PEMU_PT_GENERIC);
	*(uint16_t *)&obuf[2] = ntohs(2);
	obuf[4] = CMD_TYPE_GENERIC;
	obuf[5] = CMD_PEMU_GET_VERSION_STR;

	err = send_and_recv(PEMU_STD_PKT_SIZE, PEMU_STD_PKT_SIZE);
	if (err)
		return err;

	if (memcmp(ibuf, REP_VERSION_INFO, sizeof(REP_VERSION_INFO)) != 0) {
		log_err("communication error: can't get version msg");
		return 1;
	}

	if (extract_info(&ibuf[PEMU_CMD_REPLY_LEN], 7, version_fw)) {
		log_err("communication error: can't get version info");
		return 1;
	}

	log_info("fw version %s", version_fw);

	if (strcmp(version_fw, "9.60") != 0)
		log_wrn("please flash correct CFv234 firmware.");

	return 0;
}

int driver_pemu::probe()
{
	int err;

	log_dbg("%s() entering", __func__);

	endpoint_in = 0x81;
	endpoint_out = 0x02;

	err = libusb_open(dev, &handle);
	if (err) {
		log_err("can't create device handle, err %d: %s",
			err, libusb_strerror(err));
		return -1;
	}

	return 0;
}


