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

uint32_t bdm_ops::read_ad_reg(uint8_t reg)
{
	memset(buff, 0, 256);

	*(uint16_t *)&buff[0] = ntohs(CMD_BDMCF_RDAREG | reg);

	drv->xfer_bdm_data(buff, 2);

	return ntohl(*(uint32_t *)buff);
}
