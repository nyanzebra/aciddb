#include "RecordEvent.h"
#include "Datastore.h"

bool RecordEvent::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	switch(_type) {
		case RecordEventType::kUndefined : {
			return false;
		}
		case RecordEventType::kSet : {
			if (_args.size() != 2) { return false; }

			const std::string& key = _args[0];
			const std::string& data = _args[1];
			
			auto r = ds->createPath(key.c_str());

			if (!r) { return false; }

			auto type = r->getType();

			if (type != RecordType::kUndefined && type != RecordType::kString) {
				return false;
			}

			*r = data;

			return true;
		}
		case RecordEventType::kMove : {
			if (_args.size() != 2) { return false; }

			const std::string& key = _args[0];
			const std::string& target = _args[1];

			auto fromRecord = ds->getRecord(key.c_str());
			if (!fromRecord) { return false; }

			Record temp(std::move(*fromRecord));

			ds->removeRecord(key.c_str());
			
			auto targetRecord = ds->createPath(target.c_str());
			*targetRecord = std::move(temp);

			return true;
		}
		case RecordEventType::kDelete : {
			if (_args.size() != 1) { return false; }

			const std::string& key = _args[0];
			
			ds->removeRecord(key.c_str());

			return true;
		}
	}
}