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

			auto r = ds->getRecord(key.c_str());

			if (!r) { return NOT_FOUND_STRING; }

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

			auto r = ds->getRecord(key.c_str());
			
			if (!r) { return NOT_FOUND_STRING; }

			ds->removeRecord(key.c_str());

			return OK_STRING;
		}
		case RecordEventType::kSetCreate : {
			if (_args.size() < 2) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			const std::string& rks = { RECORD_KEY_SEPARATOR };
			std::string temp;

			ds->removeRecord(key.c_str());

			auto r = ds->createPath(key.c_str());

			if (!r) { return INVALID_PATH_STRING; }

			for (size_t i = 1; i < _args.size(); ++i) {
				auto rref = ds->createPath(key.c_str() + rks + std::to_string(i-1));

				if (!rref) { return INVALID_PATH_STRING; }

				*rref = _args[i];
			}

			return OK_STRING;
		}
		case RecordEventType::kSetInfo : {
			if (_args.size() != 1) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			const std::string& rks = { RECORD_KEY_SEPARATOR };
			std::string records;

			if (!ds->getRecord(key.c_str())) {
				return "Set Error: " + key + " is not a set.";
			}

			auto r = ds->getRecord(key.c_str());

			if (r->numChildren() == 0) { return "Empty record set!"; }

			records += "Set contains " + std::to_string(r->numChildren()) + " member(s)\n";

			if (r->numChildren() == 1) { return records + std::to_string(1) + ") " + ds->getValue(key.c_str() + rks + std::to_string(0)); }

			for (size_t i = 0; i < r->numChildren(); i++) {
				auto rvalue = ds->getValue(key.c_str() + rks + std::to_string(i));
				
				if(i != r->numChildren() - 1) {
					records += std::to_string(i + 1) + ") " + rvalue + "\n";
				}
				else {
					records += std::to_string(i + 1) + ") " + rvalue;
				}
			}

			return records;
		}
		case RecordEventType::kSetAdd : {
			if (_args.size() != 2) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			const std::string& data = _args[1];
			const std::string& rks = { RECORD_KEY_SEPARATOR };

			if(!ds->getRecord(key.c_str())) {
				return "Set Error: " + key + " is not a set.";
			}

			auto r = ds->getRecord(key.c_str());

			if(!r) { return INVALID_PATH_STRING; }

			for (size_t i = 0; i < r->numChildren(); i++) {
				if (data == ds->getValue(key.c_str() + rks + std::to_string(i))) {
					return "Record " + data + " already exists in set " + key;
				}
			}

			auto ra = ds->createPath(key.c_str() + rks + std::to_string(r->numChildren()));

			if(!ra) { return INVALID_PATH_STRING; }

			*ra = data;

			return OK_STRING;
		}
		case RecordEventType::kSetRemove : {
			if (_args.size() != 2) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			const std::string& data = _args[1];
			const std::string& rks = { RECORD_KEY_SEPARATOR };
			bool dataRemoved = false;

			auto r = ds->getRecord(key.c_str());

			if(!ds->getRecord(key.c_str())) {
				return "Set Error: " + key + " is not a set.";
			}

			const size_t count = r->numChildren();

			for (size_t i = 0; i < count; i++) {

				if (dataRemoved) {
					auto value = ds->getValue(key.c_str() + rks + std::to_string(i));
					auto temp = ds->createPath(key.c_str() + rks + std::to_string(i - 1));
					*temp = value;

					ds->removeRecord(key.c_str() + rks + std::to_string(i));
				}
				if (data == ds->getValue(key.c_str() + rks + std::to_string(i))) {
					dataRemoved = true;
					ds->removeRecord(key.c_str() + rks + std::to_string(i));
				}
			}

			if (!dataRemoved) { return "data: " + data + " is not a member of set " + key; }

			return OK_STRING;
		}
		case RecordEventType::kSetUnion : {
			if (_args.size() != 3) { return INTERNAL_ERROR_STRING; }

			const std::string& keya = _args[0];
			const std::string& keyb = _args[1];
			const std::string& keyc = _args[2];
			const std::string& rks = { RECORD_KEY_SEPARATOR };
			std::string rval;
			bool shouldAdd = false;

			auto ra = ds->getRecord(keya.c_str());

			if(!ds->getRecord(keya.c_str())) {
				return "Set Error: " + keya + " is not a set.";
			}

			auto rb = ds->getRecord(keyb.c_str());

			if(!ds->getRecord(keyb.c_str())) {
				return "Set Error: " + keyb + " is not a set.";
			}

			auto r = ds->createPath(keyc.c_str());

			if(!r) { return INVALID_PATH_STRING; }

			size_t counter = 0;

			for (; counter < ra->numChildren(); counter++) {
				auto rval = ds->getValue(keya.c_str() + rks + std::to_string(counter));
				auto rref = ds->createPath(keyc.c_str() + rks + std::to_string(counter));
				*rref = rval;
			}

			for (size_t i = 0; i < ra->numChildren(); i++) {
				shouldAdd = true;

				for (size_t j = 0; j < rb->numChildren(); j++){
					if(ds->getValue(keya.c_str() + rks + std::to_string(i)) == ds->getValue(keyb.c_str() + rks + std::to_string(j))) {
						shouldAdd = false;
						break;
					}
					rval = ds->getValue(keyb.c_str() + rks + std::to_string(j));
				}

				if(shouldAdd){
					auto rref = ds->createPath(keyc.c_str() + rks + std::to_string(counter++));

					if (!rref) { return INVALID_PATH_STRING; }

					*rref = rval;
				}
			}	

			return OK_STRING;		
		}
		case RecordEventType::kSetInter : {
			if (_args.size() != 3) { return INTERNAL_ERROR_STRING; }

			const std::string& keya = _args[0];
			const std::string& keyb = _args[1];
			const std::string& keyc = _args[2];
			const std::string& rks = { RECORD_KEY_SEPARATOR };

			auto ra = ds->getRecord(keya.c_str());

			if(!ds->getRecord(keya.c_str())) {
				return "Set Error: " + keya + " is not a set.";
			}

			auto rb = ds->getRecord(keyb.c_str());

			if(!ds->getRecord(keyb.c_str())) {
				return "Set Error: " + keyb + " is not a set.";
			}

			auto r = ds->createPath(keyc.c_str());

			if(!r) { return INVALID_PATH_STRING; }

			int counter = 0;

			for (size_t i = 0; i < ra->numChildren(); i++) {

				for (size_t j = 0; j < rb->numChildren(); j++) {

					if(ds->getValue(keya.c_str() + rks + std::to_string(i)) == ds->getValue(keyb.c_str() + rks + std::to_string(j))) {

						auto rref = ds->createPath(keyc.c_str() + rks + std::to_string(counter++));

						if(!rref) { return INVALID_PATH_STRING; }

						*rref = ds->getValue(keya.c_str() + rks + std::to_string(i));

						break;
					}
				}
			}	
			
			return OK_STRING;
		}
		case RecordEventType::kSetMember : {
			if (_args.size() != 2) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];
			const std::string& data = _args[1];
			const std::string& rks = { RECORD_KEY_SEPARATOR };

			if(!ds->getRecord(key.c_str())) {
				return "Set Error: " + key + " is not a set.";
			}

			auto r = ds->getRecord(key.c_str());

			for (size_t i = 0; i < r->numChildren(); i++) {
				if (data == ds->getValue(key.c_str() + rks + std::to_string(i))) {
					return "true";
				}
			}

			return "false";
		}
		case RecordEventType::kLeaves : {
			if (_args.size() != 1) { return INTERNAL_ERROR_STRING; }

			const std::string& key = _args[0];

			auto r = ds->getRecord(key.c_str());

			if (!r) { return NOT_FOUND_STRING; }

			if (r->getType() != RecordType::kString && r->getType() != RecordType::kUndefined) {
				return std::to_string(r->numChildren());
			} else {
				return ERROR_STRING;	
			}
		}
	}
	return "";
}