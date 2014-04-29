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

	return false;
}

bool EventDelete::operator()(Datastore* ds) const {
	if (!ds) { return false; }

	return false;
}