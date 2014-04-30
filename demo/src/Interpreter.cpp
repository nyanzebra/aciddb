#include "Interpreter.h"

void Interpreter::newInput(std::string line){
	_cli->writeLine("echo: " + line);
	// TODO: process line
}