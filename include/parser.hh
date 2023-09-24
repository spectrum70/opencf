#ifndef parser_hh
#define parser_hh

#include "bdm.hh"
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <termios.h>

using namespace std;

struct parser
{
	parser(bdm_ops *b);
	~parser();

	int run();

private:
	void prompt();
	void get_input_line(string &line);
	void process_line(string &line);
	int get_key_pressed();

	int cmd_load();
	int cmd_exit();
	int cmd_read();
	int cmd_write();
	int cmd_go();
	int cmd_step();

private:
	bdm_ops *bdm;
	vector<string> args;
	deque<string> commands;
	struct termios oldt, newt;

	typedef int(parser::*cmd)();
	map<string, cmd> mcmd;

};

#endif /* parser_hh */
