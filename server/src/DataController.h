#pragma once

#include <iostream>
#include <unordered_map>

#include "Datastore.h"
#include "Journal.h"
#include "RecordEvent.H"

typedef std::vector<std::string> Transaction;
typedef std::vector<std::string> Result;

class DataController {
public:
	DataController(std::iostream& datastoreSource, std::iostream& journalSource);

	Result processTransaction(const Transaction& transaction);

private:
	Datastore _datastore;
	Journal _journal;

	typedef std::vector<std::string> CommandArgs;

	bool _validateStatementSyntax(const std::string& command, const std::vector<std::string>& args);
};