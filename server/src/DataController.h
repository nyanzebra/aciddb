#pragma once

#include <iostream>
#include <unordered_map>

#include "../../shared/src/Transaction.h"

#include "Datastore.h"
#include "Journal.h"
#include "RecordEvent.h"

class DataController {
public:
	DataController(std::iostream& datastoreSource, std::iostream& journalSource);

	Result processTransaction(const Transaction& transaction);

	void saveDB(std::ostream& datastoreDest);
	void close();

private:
	Datastore _datastore;
	Journal _journal;

	typedef std::vector<std::string> CommandArgs;

	bool _validateStatementSyntax(const std::string& command, const std::vector<std::string>& args);
};
