#ifndef parser_hh
#define parser_hh

#include "bdm.hh"
#include <string>
#include <map>
#include <vector>

using namespace std;

struct parser
{
	parser(bdm_ops *b);

	int run();

private:
	void prompt();
	void get_input_line(string &line);
	void process_line(string &line);

	int cmd_load();
	int cmd_exit();
	int cmd_read();
	int cmd_write();

private:
	bdm_ops *bdm;
	vector<string> args;

	typedef int(parser::*cmd)();
	map<string, cmd> mcmd;
};

#endif /* parser_hh */
