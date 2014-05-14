#pragma once

#include "DBContext.h"
#include "../../shared/src/Transaction.h"

class ClientTransaction {
public:
	ClientTransaction(DBContext* context);

	void addStatement(std::string statement);
	Result commit();
	void abort();

private:
	DBContext* _context;

	bool _committed = false;
	bool _aborted = false;

	Transaction _transaction;
};