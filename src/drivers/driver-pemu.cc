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
#include <cstring>
#include <libusb-1.0/libusb.h>

using namespace trace;
using namespace utils;

static const unsigned char REP_VERSION_INFO[] = {0x99, 0x66, 0x00, 0x64};

#define CMD_PEMU_GET_VERSION_STR	0x0b
#define CMD_PEMU_BDM_REG_R		0x13

#define PEMU_CMD_REPLY_LEN		4

#define PEMU_STD_PKT_SIZE		256

#define OFFS_BDM_BUFF			5

enum pemu_commands {
	CMD_BDM_GO = 0x0c00,
	CMD_BDM_READ_CPU_AD_REG = 0x2180,
	CMD_BDM_READ_BDM_REG = 0x2d80,
	CMD_BDM_READ_SCM_REG = 0x2980,
	CMD_BDM_READ_MEM_BYTE = 0x1900,
	CMD_BDM_READ_MEM_WORD = 0x1940,
	CMD_BDM_READ_MEM_LONG = 0x1980,
	CMD_BDM_WRITE_CPU_AD_REG = 0x2080,
	CMD_BDM_WRITE_BDM_REG = 0x2c80,
	CMD_BDM_WRITE_SCM_REG = 0x2880,
	CMD_BDM_WRITE_MEM_BYTE = 0x1800,
	CMD_BDM_WRITE_MEM_WORD = 0x1840,
	CMD_BDM_WRITE_MEM_LONG = 0x1880,
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

int driver_pemu::get_programmer_info()
{
	int err;
	char version_fw[16];

	log_dbg("%s() entering", __func__);

	memset(obuf, 0, 256);

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

int driver_pemu::send_generic(uint8_t cmd_type, uint16_t len)
{
	*(uint16_t *)&obuf[0] = ntohs(PEMU_PT_CMD);
	*(uint16_t *)&obuf[2] = ntohs(len + 1);
	obuf[4] = cmd_type;

	if (send_and_recv(PEMU_STD_PKT_SIZE, PEMU_STD_PKT_SIZE) != 0)
		return 1;

	return 0;
}

uint32_t driver_pemu::bdm_read_ad_reg(uint8_t reg)
{
	uint8_t *buff = &obuf[OFFS_BDM_BUFF];

	buff[0] = CMD_PEMU_BDM_REG_R;
	*(uint16_t *)&buff[1] = ntohs(CMD_BDM_READ_CPU_AD_REG | reg);

	memset(ibuf, 0, 256);

	send_generic(CMD_TYPE_DATA, 3);

	for (int i = 0; i < 64; ++i) {
		printf("%02x ", (int)ibuf[i] & 0xff);
	}

	printf("\n");

	return ntohl(*(uint32_t *)ibuf);
}

int driver_pemu::check_connected_cpu()
{
	uint32_t d0, d1;

	d0 = bdm_read_ad_reg(CF_D0);
	d1 = bdm_read_ad_reg(CF_D1);

	printf("%08x d0, %08x d1", d0, d1);

	if (d0 == 0xffffffff || d1 == 0xffffffff)
		return -1;

	if ((d0 & 0x0f) != 0xf)
		return -1;

	if (((d0 & 0xf0) >> 4) != 0x02)
		return -1;

	if (!(d0 & (1 << 11)))
		return -1;

	if (((d0 & 0xf00000) >> 20) != 0x4)
		return -1;

	log_info("mcf5441x detected, D+PSTB, MMU, ISA_C");

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

	err = libusb_reset_device(handle);
	if(err) {
		log_err("cannot reset device, err %d: %s",
			err, libusb_strerror(err));
		return -1;
	}

	return 0;
}


