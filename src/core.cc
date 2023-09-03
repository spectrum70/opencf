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

#include "core.hh"
#include "bdm.hh"
#include "coldfire.hh"
#include "parser.hh"
#include "trace.hh"

#include <unistd.h>

using namespace trace;

core::core() : bdm(0)
{
}

core::~core()
{
	if (bdm)
		delete(bdm);
}

int core::get_cpu_info()
{
	cpu_info cpu;
	char isa[3] = {0};

	bdm->reset(true);
	usleep(100000);
	bdm->reset(false);
	usleep(100000);

	cpu.d0.reg = bdm->read_ad_reg(CF_D0);
	cpu.d1.reg = bdm->read_ad_reg(CF_D1);

	log_dbg("%s() d0 %08x, d1 %08x", __func__, cpu.d0.reg, cpu.d1.reg);

	if (cpu.d0.f.magic != 0xcf) {
		return -1;
	}

	isa[0] = (cpu.d0.f.isa_rev & 7) + 0x61;
	if (cpu.d0.f.isa_rev & 8)
		isa[1] = '+';

	log_info("found: %s, v.%d rev.%d %sisa %s",
		 (cpu.d0.f.magic == 0xcf ? "coldfire" : "unknown"),
		 cpu.d0.f.version,
		 cpu.d0.f.revision,
		 (cpu.d0.f.mmu ? "mmu " : ""),
		 isa);

	return 0;
}

int core::get_programmer_info()
{
	return drv->get_programmer_info();
}

int core::run()
{
	int err;

	log_dbg("%s() core running", __func__);

	if (dc.init())
		return 1;

	drv = dc.get_current_driver();

	bdm = new bdm_ops(drv);
	if (!bdm) {
		log_err("cannot create bdm, exiting");
		return 1;
	}

	err = get_programmer_info();
	if (err) {
		log_err("cannot read programmer info, exiting");
		return 1;
	}

	log_info("detecting connected cpu ...");
	err = get_cpu_info();
	if (err) {
		log_err("cpu not found, please check the cable, "
			"cpu solderings,\nboard power, "
			"and/or reset the programmer.");
		return 1;
	}

	parser p(bdm);

	return p.run();
}
