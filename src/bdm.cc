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

#include "bdm.hh"
#include "utils.hh"
#include "driver-core.hh"

#include <cstring>

using namespace utils;

bdm_ops::bdm_ops(driver *current_driver) : drv(current_driver)
{
}

void bdm_ops::reset(bool state)
{
	drv->send_reset(state);
}

void bdm_ops::go()
{
	drv->send_go();
}

uint32_t bdm_ops::read_dm_reg(uint8_t reg)
{
	memset(buff, 0, 2);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_RDMREG | reg);

	drv->xfer_bdm_data(buff, 2);

	return ntohl(*(uint32_t *)buff);
}

uint32_t bdm_ops::write_dm_reg(uint8_t reg, uint32_t value)
{
	memset(buff, 0, 6);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_WDMREG | reg);
	*(uint32_t *)&buff[2] = ntohl(value);

	drv->xfer_bdm_data(buff, 6);

	return ntohl(*(uint32_t *)buff);
}

uint32_t bdm_ops::read_ad_reg(uint8_t reg)
{
	memset(buff, 0, 2);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_RDAREG | reg);

	drv->xfer_bdm_data(buff, 2);

	return ntohl(*(uint32_t *)buff);
}

uint32_t bdm_ops::write_ad_reg(uint8_t reg, uint32_t value)
{
	memset(buff, 0, 6);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_WDAREG | reg);
	*(uint32_t *)&buff[2] = ntohl(value);

	drv->xfer_bdm_data(buff, 6);

	return ntohl(*(uint32_t *)buff);;
}

uint32_t bdm_ops::read_mem_byte(uint32_t address)
{
	memset(buff, 0, 6);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_RD_MEM_B);
	*(uint32_t *)&buff[2] = ntohl(address);

	drv->xfer_bdm_data(buff, 6);

	return ntohl(*(uint32_t *)buff);
}

uint32_t bdm_ops::write_mem_byte(uint32_t address, uint8_t value)
{
	memset(buff, 0, 10);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_WR_MEM_B);
	*(uint32_t *)&buff[2] = ntohl(address);

	/* heh, pemu wants a 16 bit value here, and 10 total to send */
	*(uint16_t *)&buff[6] = ntohs(value);

	drv->xfer_bdm_data(buff, 10);

	return 0;
}

uint32_t bdm_ops::read_ctrl_reg(cr_type type)
{
	memset(buff, 0, 6);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_RCREG);
	*(uint32_t *)&buff[2] = ntohl(type);

	drv->xfer_bdm_data(buff, 6);

	return ntohl(*(uint32_t *)buff);
}

uint32_t bdm_ops::write_ctrl_reg(cr_type type, uint32_t value)
{
	memset(buff, 0, 10);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_WCREG);
	*(uint32_t *)&buff[2] = ntohl(type);
	*(uint32_t *)&buff[6] = ntohl(value);

	drv->xfer_bdm_data(buff, 10);

	return 0;
}

uint32_t bdm_ops::step()
{
	int value;
	uint32_t rval;

	switch (state) {
	case st_halted:
		/*
		 * getting CRS and re-setting for step            *
		 */
		value = read_dm_reg(BDM_REG_CSR);
		/* setting in step, no interrupt, emulator mode */
		value |= (CSR_SSM | CSR_IPI | CSR_EMULATION);
		write_dm_reg(BDM_REG_CSR, value);
		state = st_step;
		/* FALLTROUGH */
	case st_step:
		drv->send_go();
		rval = read_ctrl_reg(crt_pc);
		break;
	case st_running:
		rval = -1;
		break;
	}

	return rval;
}

/*
 * Write a memory buffer to a specific location
 *
 * P&E m.u. has propertary all-inclusive bdm-father commands for memory write,
 * i am assuming they are more performant then their bdm dump/write friends,
 * so i am using them
 */
int bdm_ops::load_segment(uint8_t *data, uint32_t dest, uint32_t size)
{
	return drv->send_big_block(data, dest, size);
}
