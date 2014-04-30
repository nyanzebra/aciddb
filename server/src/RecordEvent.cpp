#include "RecordEvent.h"
#include "Datastore.h"

bool EventSet::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	auto r = ds->createPath(key.c_str());

	if (!r) {
		return false;
	}

	auto type = r->getType();
	if (type != RecordType::kUndefined && type != RecordType::kString) {
		return false;
	}

	(*r) = data;

	return true;
}

bool EventMove::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	auto from = ds->getRecord(key.c_str());
	if (!from) { return false; }

	Record temp(std::move(*from));

	ds->removeRecord(key.c_str());
	
	auto targetRecord = ds->createPath(target.c_str());

	*targetRecord = std::move(temp);

	return true;
}

bool EventDelete::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	ds->removeRecord(key.c_str());

	return true;
}