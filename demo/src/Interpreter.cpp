#include "../../cpp-connector/src/DBContext.h"
#include "../../cpp-connector/src/ClientTransaction.h"

#include "../../shared/src/utility.h"

#include "Interpreter.h"

Interpreter::Interpreter(std::shared_ptr<CLI> cli, DBContext* context)
	: _cli(cli)
	, _context(context)
{}

void Interpreter::newInput(std::string line){
	ClientTransaction txn(_context);

	auto statements = Split(line, ';');

	for (auto&& s : statements) {
		txn.addStatement(std::move(s));	
	}

	try {
		auto result = txn.commit();

		for (auto&& r : result) {
		 	_cli->writeLine(r);
		}
	} catch (std::exception &e) {
		_cli->writeLine(e.what());
	}
}