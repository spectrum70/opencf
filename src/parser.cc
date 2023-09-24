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
#include "utils.hh"
#include "trace.hh"
#include "elf.hh"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace trace;
using namespace utils;

enum kstate {
	kst_normal,
	kst_ctrl,
	kst_excape_sec,
};

parser::parser(bdm_ops *b): bdm(b)
{
	mcmd["load"] = &parser::cmd_load;
	mcmd["exit"] = &parser::cmd_exit;
	mcmd["quit"] = &parser::cmd_exit;
	mcmd["read"] = &parser::cmd_read;
	mcmd["write"] = &parser::cmd_write;
	mcmd["go"] = &parser::cmd_go;
	mcmd["step"] = &parser::cmd_step;
	mcmd["st"] = &parser::cmd_step;

	tcgetattr(STDIN_FILENO, &oldt);

	newt = oldt;
	newt.c_lflag &= ~(ICANON);
	newt.c_lflag &= ~(ECHO);

	/* Disable canonical mode */
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
}

parser::~parser()
{
	/* Back to old config */
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int parser::cmd_load()
{
	char *edata;
	elf e(bdm);

	edata = e.load_elf(args[0]);
	if (!edata)
		return 1;

	return 0;
}

int parser::cmd_go()
{
	bdm->go();

	return 0;
}

int parser::cmd_step()
{
	bdm->step();

	return 0;
}

int parser::cmd_exit()
{
	exit(0);
}

int parser::cmd_read()
{
	int rval = -1;

	if (args.size() < 2)
		return rval;

	if (args[0] == "reg") {
		if (args[1] == "rambar") {
			rval = bdm->read_ctrl_reg(crt_rambar);
		} else if (args[1] == "pc") {
			rval = bdm->read_ctrl_reg(crt_pc);
		} else {
			int reg_type = args[1][0];
			int reg = args[1][1];

			if (reg < '0' || reg > '7')
				return -1;

			reg = reg - '0';

			switch(reg_type) {
			case 'd':
			case 'D':
				reg += CF_D0;
				break;
			case 'a':
			case 'A':
				reg += CF_A0;
				break;
			default:
				return -1;
			}

			rval = bdm->read_ad_reg(reg);
		}

		printf("%08x\n", rval);

	} else if (args[0] == "mem.b") {
		string address = args[1];
		int addr;

		addr = str_to_bin(address);

		int rval = bdm->read_mem_byte(addr);

		printf("%08x\n", rval);
		printf("%02x\n", (rval >> 16) & 0xff);
	} else
		return 1;

	return 0;
}

int parser::cmd_write()
{
	int rval = -1;
	int val;

	if (args.size() < 3)
		return 1;

	if (args[0] == "reg") {
		val = str_to_bin(args[2]);
		if (args[1] == "rambar") {
			rval = bdm->write_ctrl_reg(crt_rambar, val);
		} else if (args[1] == "pc") {
			rval = bdm->write_ctrl_reg(crt_pc, val);
		}
	} else if (args[0] == "mem.b") {
		string address = args[1];
		string value = args[2];
		int addr;

		addr = str_to_bin(address);
		val = str_to_bin(value);

		log_dbg("val = %d\n", val);

		if (val > 255)
			return 1;

		rval = bdm->write_mem_byte(addr, val);
	}

	return rval;
}

void parser::prompt()
{
	cout << "§ " << flush;
}

int parser::get_key_pressed()
{
	return getchar();
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

	if ((this->*mcmd[cmd])()) {
		log_err("invalid command");
	}
}

void parser::get_input_line(string &line)
{
	int c;
	int kstate = kst_normal;
	int ptr = -1, i, pos, llen = 0;

	line.clear();

	for (;;) {
		c = get_key_pressed();
		switch (kstate) {
		case kst_normal:
			/* ESC ? */
			if (c == 27) {
				kstate = kst_ctrl;
				break;
			}
			/* We ECHO the char */
			cout << (char)c;
			if (c != '\n') {
				line.push_back(c);
			} else {
				goto line_in;
			}
			break;
		case kst_ctrl:
			kstate = (c == '[') ? kst_excape_sec : kst_normal;
			break;
		case kst_excape_sec:
			if (c == 'A' || c == 'B') {
				int size = commands.size();

				if (!size)
					continue;

				if (llen) {
					for (i = 0; i < llen; ++i)
						cout << '\b';
				}

				if (c == 'B') {
					if (ptr)
						ptr--;
				} else {
					if (ptr < (size - 1))
						ptr++;
				}
				pos = commands.size() - ptr - 1;
				line = commands[pos];
				llen = line.size();

				cout << line << flush;
			}

			kstate = kst_normal;
			break;
		}
	}

line_in:
	llen = line.size();
	/* Store into lines buffer */
	commands.push_back(line);
}

int parser::run()
{
	string line;

	log_info("starting parser ...");

	for (;;) {
		prompt();
		get_input_line(line);
		if (line.size())
			process_line(line);
	}
}
