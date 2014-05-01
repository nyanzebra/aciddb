#pragma once

#include "CLI.h"

class DBContext;

class Interpreter : public CLI::Observer {
public:
	Interpreter(std::shared_ptr<CLI> cli, DBContext* context);

	void newInput(std::string line);

private:
	std::shared_ptr<CLI> _cli;
	DBContext* _context;
};