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


#include <iostream>
#include <getopt.h>

#include "getopts.hh"
#include "version.hh"

using namespace std;

void getopts::info()
{
	cout << "opencf" << " v." << version << "\n";
}

void getopts::usage()
{
	info();
	cout << "(C) 2023, kernel-space.org\n"
	     << "Usage: opencf [OPTION]\n"
	     << "Example: ./opencf -v\n"
	     << "Options:\n"
	     << "  -h,  --help        this help\n"
	     << "  -p,  --path        server root path (def. /srv/tftp)\n"
	     << "  -V,  --version     program version\n"
	     << "  -v                 verbose\n"
	     << "\n";
}

void getopts::defaults()
{
	opts::get().server_path = "/srv/tftp";
}

getopts::getopts(int argc, char **argv)
{
	int c;

	defaults();

	for (;;) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", no_argument, 0, 'h'},
			{"version", no_argument, 0, 'V'},
			{"path", required_argument, 0, 'p'},
			{"", no_argument, 0, 'v'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "hvVp:",
				long_options, &option_index);

		if (c == -1) {
			if (optind < argc) {
				while (optind < argc)
					opts::get().nonopts.
						push_back(argv[optind++]);
				break;
			}
			return;
		}

		switch (c) {
		case 'h':
			usage();
			exit(-1);
			break;
		case 'v':
			opts::get().verbose = true;
			break;
		case 'V':
			info();
			exit(-1);
			break;
		case 'p':
			opts::get().server_path = optarg;
			break;
		default:
			exit(-2);
		}
	}
}
