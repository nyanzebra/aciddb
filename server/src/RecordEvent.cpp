#ifndef SERVER_PCH_H
#include "pch.h"
#endif
#include "RecordEvent.h"
#include "Datastore.h"

std::string RecordEvent::operator()(Datastore* ds) const {
	if (!ds) { return INTERNAL_ERROR_STRING; }

	switch(_type) {
		case RecordEventType::kUndefined : {
			return ERROR_STRING;
		}
		case RecordEventType::kGet : {
			if (_args.size() != 1) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];

			return ds->getValue(key.c_str());
		}
		case RecordEventType::kSet : {
			if (_args.size() != 2) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			const std::string& data = _args[1];
			
			auto r = ds->createPath(key.c_str());

			if (!r) { return INVALID_PATH_STRING; }

			auto type = r->getType();

			if (type != RecordType::kUndefined && type != RecordType::kString) {
				return ERROR_STRING;
			}

			*r = data;

			return OK_STRING;
		}
		case RecordEventType::kMove : {
			if (_args.size() != 2) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			const std::string& target = _args[1];

			auto fromRecord = ds->getRecord(key.c_str());
			if (!fromRecord) { return NOT_FOUND_STRING; }

			Record temp(std::move(*fromRecord));

			ds->removeRecord(key.c_str());
			
			auto targetRecord = ds->createPath(target.c_str());
			*targetRecord = std::move(temp);

			return OK_STRING;
		}
		case RecordEventType::kDelete : {
			if (_args.size() != 1) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			
			ds->removeRecord(key.c_str());

			return OK_STRING;
		}
	}
	return "";
}