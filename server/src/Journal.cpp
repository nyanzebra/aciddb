#include "Journal.h"

#include <sstream>

void Journal::writeTransaction(const std::vector<RecordEvent>& events) {
	if (!_dataStream) {
		throw std::runtime_error("no datastream connected to journal");
	}

	std::stringstream ss;

	{
		OutputArchiveType oarch(ss);
		oarch << events;
	}

	*_dataStream << ss.rdbuf();
	_dataStream->flush();
}

bool Journal::applyToDatastore(Datastore* ds) {
	if (!_dataStream) {
		throw std::runtime_error("no datastream connected to journal");
	}

	std::vector<std::vector<RecordEvent>> events;

	try {
		InputArchiveType iarch(*_dataStream);
		while (true) {
			std::vector<RecordEvent> e;
			iarch >> e;
			events.push_back(std::move(e));
		}
	} catch (...) {}

	for (auto&& vec : events) {
		for (auto&& e : vec) {
			e(ds);
		}
	}

	return true;
}