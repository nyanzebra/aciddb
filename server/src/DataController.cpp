#ifndef SERVER_PCH_H
#include "pch.h"
#endif
#include "DataController.h"
#include "RecordEvent.h"

DataController::DataController(std::iostream& datastoreSource, std::iostream& journalSource)
	: _datastore(datastoreSource)
	, _journal(&journalSource)
{}

void DataController::saveDB(std::ostream& datastoreDest) {
	_datastore.write(datastoreDest);
}

void DataController::close() {
	_journal.close();
}

Result DataController::processTransaction(const Transaction& transaction) {

	Result results(transaction.size(), "NOT EXECUTED");

	std::vector<std::string> allCommands(transaction.size());
	std::vector<std::vector<std::string>> allArgs(transaction.size());

	bool syntaxCheck = true;
	size_t size = transaction.size();
	for (size_t i = 0; i < size; ++i) {
		auto args = Split(transaction[i], ' ');
		if (args.empty()) {
			results[i] = "INVALID SYNTAX";
			syntaxCheck = false;
		}

		allCommands[i] = std::move(args[0]);
		args.erase(args.begin()); // remove first parameter, leave args

		allArgs[i] = std::move(args);

		if (!_validateStatementSyntax(allCommands[i], allArgs[i])) {
			results[i] = "INVALID SYNTAX";
			syntaxCheck = false;
		}
	}

	if (!syntaxCheck) {
		return results;
	}

	std::vector<RecordEvent> events;

	for (size_t i = 0; i < transaction.size(); ++i) {
		auto& command = allCommands[i];
		auto& args = allArgs[i];

		if (command == "get") {
			events.emplace_back(RecordEventType::kGet, std::move(args));
		} else if (command == "set") {
			events.emplace_back(RecordEventType::kSet, std::move(args));
		} else if (command == "move") {
			events.emplace_back(RecordEventType::kMove, std::move(args));
		} else if (command == "delete") {
			events.emplace_back(RecordEventType::kDelete, std::move(args));
		} else if (command == "setcreate") {
			events.emplace_back(RecordEventType::kSetCreate, std::move(args));
		} else if (command == "setinfo") {
			events.emplace_back(RecordEventType::kSetInfo, std::move(args));
		} else if (command == "setadd") {
			events.emplace_back(RecordEventType::kSetAdd, std::move(args));
		} else if (command == "setremove") {
			events.emplace_back(RecordEventType::kSetRemove, std::move(args));
		} else if (command == "setunion") {
			events.emplace_back(RecordEventType::kSetUnion, std::move(args));
		} else if (command == "setinter") {
			events.emplace_back(RecordEventType::kSetInter, std::move(args));
		} else if (command == "setmember") {
			events.emplace_back(RecordEventType::kSetMember, std::move(args));
		} else if (command == "leaves") {
			events.emplace_back(RecordEventType::kLeaves, std::move(args));
		}
	}

	_journal.writeTransaction(events);

	size = events.size();
	for (size_t i = 0; i < events.size(); ++i) {
		results[i] = events[i](&_datastore);
	};

	return results;
}

bool DataController::_validateStatementSyntax(const std::string& command, const std::vector<std::string>& args) {

	size_t size = args.size();
	return !(false
		|| (command == "get" && size != 1)
	    || (command == "set" && size != 2)
		|| (command == "move" && size != 2)
		|| (command == "delete" && size != 1)
		|| (command == "setcreate" && size < 1)
		|| (command == "setinfo" && size != 1)
		|| (command == "setadd" && size != 2)
		|| (command == "setremove" && size != 2)
		|| (command == "setunion" && size != 3)
		|| (command == "setinter" && size != 3)
		|| (command == "setmember" && size != 2)
		|| (command == "leaves" && size != 1)
	);
}