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
#include <iomanip>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

using namespace trace;
using namespace utils;
using namespace std;

enum kstate {
	kst_normal,
	kst_ctrl,
	kst_excape_sec,
};

static constexpr char special_regs[] = "pc, vbr, rambar, sp, sr";

parser_help::parser_help()
{
	mcmd_help["exit"] = "exit application";
	mcmd_help["go"] = "execute continuously";
	mcmd_help["halt"] = "stop execution";
	mcmd_help["help"] = "this help";
	mcmd_help["load"] = "load elf executable";
	mcmd_help["quit"] = "exit alias, exit application";
	mcmd_help["read"] = "read memory or register:\n"
		"    read mem.b location    read one byte from memory\n"
		"    read mem.w location    read two bytes from memory\n"
		"    read mem.l location    read four bytes from memory\n"
		"    read reg name          read special register\n"
		"    special registers: ";
	mcmd_help["read"] += special_regs;
	mcmd_help["regs"] = "dump cpu registers";
	mcmd_help["st"] = "step alias, shorted";
	mcmd_help["step"] = "step";
	mcmd_help["write"] = "write memory or register:\n"
		"    write mem.b location val    write one byte to memory\n"
		"    write mem.w location val    write two bytes to memory\n"
		"    write mem.l location val    write four bytes to memory\n"
		"    write reg name val          write special register\n"
		"    special registers: ";
	mcmd_help["write"] += special_regs;
}

parser::parser(bdm_ops *b): bdm(b)
{
	mcmd["exit"] = &parser::cmd_exit;
	mcmd["go"] = &parser::cmd_go;
	mcmd["halt"] = &parser::cmd_halt;
	mcmd["help"] = &parser::cmd_help;
	mcmd["load"] = &parser::cmd_load;
	mcmd["quit"] = &parser::cmd_exit;
	mcmd["read"] = &parser::cmd_read;
	mcmd["regs"] = &parser::cmd_dump_cpu_regs;
	mcmd["st"] = &parser::cmd_step;
	mcmd["step"] = &parser::cmd_step;
	mcmd["write"] = &parser::cmd_write;

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

int parser::cmd_help()
{
	log_ansi(ANSI_BOLD, "Available commands:");

	map<string, cmd>::iterator it;

	for (it = mcmd.begin(); it != mcmd.end(); it++) {
		log_ansi(ANSI_BOLD, it->first.c_str());
		string help = string("  ") + mcmd_help[it->first.c_str()];
		log_ansi(ANSI_COLOR_RESET, help.c_str());
	}

	log_ansi(ANSI_BOLD, "Keys:");
	log_ansi(ANSI_BOLD, "key enter");
	log_ansi(ANSI_COLOR_RESET, "  repeat last command");

	return 0;
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

int parser::cmd_halt()
{
	bdm->halt();

	return 0;
}

int parser::cmd_step()
{
	uint32_t rval;

	rval = bdm->step();

	if (rval != 0xffffffff) {
		/* not running, show pc */
		log_info("pc: %08x", rval);
	}

	return 0;
}

int parser::cmd_exit()
{
	exit(0);
}

void parser::dump_set(stringstream &ss, int reg, char pre)
{
	int i, rval;

	for (i = 0; i < 4; ++i, reg++) {
		rval = bdm->read_ctrl_reg((cr_type)reg);
		ss << pre << i << " " << hex
		<< setw(8) << setfill('0') << rval << " ";
	}
	ss << "\n";
	for (i = 0; i < 4; ++i, reg++) {
		rval = bdm->read_ctrl_reg((cr_type)reg);
		ss << pre << i + 4 << " " << hex
		<< setw(8) << setfill('0') << rval << " ";
	}
	ss << "\n";
}

int parser::cmd_dump_cpu_regs()
{
	stringstream ss;

	ss << ANSI_COLOR_WHITE;
	dump_set(ss, crt_d0_r, 'd');
	ss << ANSI_COLOR_CYAN;
	dump_set(ss, crt_a0_r, 'a');

	ss << ANSI_COLOR_YELLOW
	   << "pc " << setw(8) << setfill('0')
	   << bdm->read_ctrl_reg(crt_pc) << " "
	   << "sr " << setw(8) << setfill('0')
	   << bdm->read_ctrl_reg(crt_sr) << " "
	   << "fp " << setw(8) << setfill('0')
	   << bdm->read_ctrl_reg(crt_fp_r) << " "
	   << "sp " << setw(8) << setfill('0')
	   << bdm->read_ctrl_reg(crt_sp_r) << "\n"
	   << ANSI_COLOR_GREEN
	   << "rambar " << setw(8) << setfill('0')
	   << bdm->read_ctrl_reg(crt_rambar) << "\n"
	   << "vbr    " << setw(8) << setfill('0')
	   << bdm->read_ctrl_reg(crt_vbr);

	log_info(ss.str().c_str());

	return 0;
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
		} else if (args[1] == "vbr") {
			rval = bdm->read_ctrl_reg(crt_vbr);
		} else if (args[1] == "sp") {
			rval = bdm->read_ctrl_reg(crt_sp_r);
		} else if (args[1] == "sr") {
			rval = bdm->read_ctrl_reg(crt_sr);
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

		rval = bdm->read_mem_byte(addr);

		printf("%02x\n", (rval >> 16) & 0xff);
	} else if (args[0] == "mem.w") {
		string address = args[1];
		int addr;

		addr = str_to_bin(address);

		rval = bdm->read_mem_word(addr);

		printf("%04x\n", (rval >> 16) & 0xffff);
	} else if (args[0] == "mem.l") {
		string address = args[1];
		int addr;

		addr = str_to_bin(address);

		rval = bdm->read_mem_long(addr);

		printf("%08x\n", rval);
	} else
		return 1;

	return 0;
}

void parser::get_mem_values(uint32_t &addr, uint32_t &val)
{
	string address = args[1];
	string value = args[2];

	addr = str_to_bin(address);
	val = str_to_bin(value);
}

int parser::cmd_write()
{
	int rval = -1;
	uint32_t addr, val;

	if (args.size() < 3)
		return 1;

	if (args[0] == "reg") {
		val = str_to_bin(args[2]);
		if (args[1] == "rambar") {
			rval = bdm->write_ctrl_reg(crt_rambar, val);
		} else if (args[1] == "pc") {
			rval = bdm->write_ctrl_reg(crt_pc, val);
		} else if (args[1] == "vbr") {
			rval = bdm->write_ctrl_reg(crt_vbr, val);
		} else if (args[1] == "sp") {
			rval = bdm->write_ctrl_reg(crt_sp_w, val);
		}
	} else if (args[0] == "mem.b") {
		get_mem_values(addr, val);

		if (val > 255)
			return 1;

		rval = bdm->write_mem_byte(addr, (uint16_t)val);
	} else if (args[0] == "mem.w") {
		get_mem_values(addr, val);

		if (val > 65535)
			return 1;

		rval = bdm->write_mem_word(addr, (uint16_t)val);
	} else if (args[0] == "mem.l") {
		get_mem_values(addr, val);

		rval = bdm->write_mem_long(addr, val);
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

	/* Store last. */
	last = line;

	args.clear();

	/* Check for comments */

	pos = line.find('#');
	if (pos != (size_t)-1)
		line.resize(pos);

	if (!line.size()) {
		printf("enter pressed\n");
	}

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

void parser::repeat_last_cmd()
{
	if (last.size())
		process_line(last);
}

void parser::get_input_line(string &line)
{
	int c;
	int kstate = kst_normal;
	int ptr = -1, i, pos, llen = 0;
	string tmp;

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
			if (c == 127) {
				if (line.size()) {
					cout << "\b \b" << flush;
					line.resize(line.size() - 1);
					if (line_pos)
						line_pos--;
				}
			} else {
				/* We ECHO the char */
				cout << (char)c;
				if (c != '\n') {
					if (line_pos && line_pos < line.size()) {
						tmp = line.substr(line_pos);
						line.insert(line_pos, 1, c);
						cout << tmp;
						cout << "\x1b[" << (line.size() - line_pos - 1) << "D";
					} else {
						line.push_back(c);

					}
					line_pos++;
				} else {
					line_pos = 0;
					goto line_in;
				}
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
						cout << "\b \b";
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
				line_pos = line.size();

			} else if (c == 'D') {
				if (line_pos > 1) {
					cout << "\x1b[D";
					line_pos--;
				}
			} else if (c == 'C') {
				if (line_pos && line_pos < line.size()) {
					cout << "\x1b[C";
					line_pos++;
				}
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
		else
			repeat_last_cmd();
	}
}
