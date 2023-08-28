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

#include "trace.hh"
#include "getopts.hh"
#include "version.hh"

#include "driver-core.hh"

using namespace trace;

int main(int argc, char **argv)
{
	getopts opts(argc, argv);

	log_info("opencf " version " starting\n", argv[0]);
	log_info("starting driver core ...");

	driver_core dc;

	dc.init();
}
