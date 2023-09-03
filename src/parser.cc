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

#include "parser.hh"
#include "trace.hh"
#include "elf.hh"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace trace;

parser::parser(bdm_ops *b): bdm(b)
{
	mcmd["load"] = &parser::cmd_load;
	mcmd["exit"] = &parser::cmd_exit;
}

void parser::cmd_load()
{
	char *edata;
	elf e;

	edata = e.load_elf(args[0]);
	if (!edata)
		return;
}

void parser::cmd_exit()
{
	exit(0);
}

void parser::prompt()
{
	cout << "§ " << flush;
}

void parser::process_line(string &line)
{
	string cmd;
	size_t pos;

	args.clear();

	/* Check for comments */

	pos = line.find('#');
	if (pos != (size_t)-1)
		line.resize(pos);

	/* TO DO, check for ; */

	while (line.size()) {
		pos = line.find(' ');
		if (pos == string::npos) {
			args.push_back(line);
			break;
		} else {
			args.push_back(line.substr(0, pos));
			line.erase(0, pos + 1);
		}
	}

	cmd = args[0];
	args.erase(args.begin());

	if (mcmd.find(cmd) == mcmd.end()) {
		log_err("command not found");
		return;
	}

	(this->*mcmd[cmd])();
}

void parser::get_input_line(string &line)
{
	getline(cin, line);
}

int parser::run()
{
	string line;

	log_info("starting parser ...");

	for (;;) {
		prompt();
		get_input_line(line);
		process_line(line);
	}
}
