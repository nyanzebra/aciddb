#include "RecordEvent.h"
#include "Datastore.h"

bool EventSet::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	auto r = ds->createPath(getKey().c_str());

	if (!r) {
		return false;
	}

	auto type = r->getType();
	if (type != RecordType::kUndefined && type != RecordType::kString) {
		return false;
	}

	(*r) = _data;

	return true;
}

bool EventMove::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	auto from = ds->getRecord(getKey().c_str());
	if (!from) { return false; }

	Record temp(std::move(*from));

	ds->removeRecord(getKey().c_str());
	
	auto target = ds->createPath(_target.c_str());
	*target = std::move(temp);

	return true;
}

bool EventDelete::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	ds->removeRecord(getKey().c_str());

	return true;
}