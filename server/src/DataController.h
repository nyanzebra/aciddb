#pragma once
#include "Datastore.h"
#include "Journal.h"

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