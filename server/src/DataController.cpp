#include "DataController.h"

#include "../../shared/src/utility.h"

DataController::DataController(std::iostream& datastoreSource, std::iostream& journalSource)
	: _datastore(datastoreSource)
	, _journal(journalSource)
{}

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
	);
}