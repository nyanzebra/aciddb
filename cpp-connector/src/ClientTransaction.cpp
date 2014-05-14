#include "ClientTransaction.h"

ClientTransaction::ClientTransaction(DBContext* context)
	: _context(context)
{}

void ClientTransaction::addStatement(std::string statement) {
	_transaction.push_back(std::move(statement));
}

Result ClientTransaction::commit() {
	if (!_context) {
		throw std::runtime_error("invalid context");
	}
	if (_committed) {
		throw std::runtime_error("transaction already committed");
	}
	if (_aborted) {
		throw std::runtime_error("transaction already aborted");
	}
	_committed = true;

	return _context->send(_transaction);
}

void ClientTransaction::abort() {
	_aborted = true;
	_transaction.clear();
}