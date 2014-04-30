#pragma once

#include "CLI.h"

class Interpreter : public CLI::Observer {
private:
	std::shared_ptr<CLI> _cli;

public:
	Interpreter(std::shared_ptr<CLI> cli) : _cli(cli) {}

	void newInput(std::string line);
};