#include "Datastore.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <vector>

static const std::string gEmptyString;

Datastore::Datastore(std::istream& in) {
	boost::archive::text_iarchive iarch(in);
	try {
		iarch >> _root;	
	} catch (...) {
		_good = false;
	}
}

bool Datastore::write(std::ostream& out) {
	if (!_good) {
		return false;
	}

	try {

		boost::archive::text_oarchive oarch(out);
		oarch << _root;

		return true;
	} catch (...) {}
	return false;
}

std::vector<std::string> Datastore::_tokenizePath(const char* path) {
	const char* pos = path;

	bool escaped = false;

	std::vector<std::string> ret = {""};

	while (*pos != '\0') {
		if (escaped) {
			switch (*pos) {
				case '\\':
				case ':':
					ret.back().push_back(*pos);
			}
			escaped = false;
		} else if (*pos == '\\') {
			escaped = true;
		} else if (*pos == ':') {
			// no part of the path should be empty
			if (ret.back() == "") {
				return {};
			}
			ret.push_back("");
		} else {
			ret.back().push_back(*pos);
		}
		++pos;
	}
	// no part of the path should be empty
	if (ret.size() == 1 && ret.back() == "") {
		return {};
	}
	return ret;
}

void Datastore::removeRecord(const char* path) {

	auto tokens = _tokenizePath(path);
	if (tokens.empty()) { return; } // invalid path

	auto records = _recordsOnPath(tokens);
	if (records.empty()) {
		return; }

	// drop the tokens aren't represented by records
	tokens.resize(records.size());

	if (records.size() == 1) {
		// a child of root is being removed
		_root.removeChild(tokens.back().c_str());
		return;
	}

	records.pop_back();
	Record* parent = records.back();
	records.pop_back();

	try {
		parent->removeChild(tokens.back().c_str());
		tokens.pop_back();

		// prune stale records in this branch
		while (records.size() > 0) {
			Record* child = parent;
			parent = records.back();
			records.pop_back();

			if (child->numChildren() == 0) {
				parent->removeChild(tokens.back().c_str());
				tokens.pop_back();
			}
		}
	} catch(std::exception & e) {
		printf("%s\n", e.what());
	}
}

std::vector<Record*> Datastore::_recordsOnPath(const std::vector<std::string>& path) {

	if (path.empty()) { return {}; } // invalid path

	std::vector<Record*> ret;

	Record* r = &_root;

	for (auto&& token : path) {
		try {
			r = &r->getChild(token.c_str());
		} catch (...) {
			return ret;
		}
		auto type = r->getType();
		if (type == RecordType::kUndefined) {
			return ret;
		}
		ret.push_back(r);
		if (type == RecordType::kString) {
			return ret;
		}
	}
	return ret;
}

const std::string& Datastore::getValue(const char* key) {
	if (_good == false) {
		return gEmptyString;
	}
	auto r = getRecord(key);
	if (r && r->getType() == RecordType::kString) {
		return r->getVal();
	}
	return gEmptyString;
}

// TODO: method for checking records without creating records

Record* Datastore::getRecord(const char* path) {

	// TODO: string operations like this are expensive, optimize?
	auto tokens = _tokenizePath(path);
	if (tokens.empty()) { return nullptr; } // invalid path

	Record* r = &_root;

	try {
		for (auto&& token : tokens) {
			r = &r->getChild(token.c_str());
			if (r->getType() == RecordType::kUndefined) {
				return nullptr;
			}
		}
	} catch (...) {
		return nullptr;
	}
	return r;
}

Record* Datastore::createPath(const char* path) {

	// TODO: string operations like this are expensive, optimize?
	auto tokens = _tokenizePath(path);
	if (tokens.empty()) { return nullptr; } // invalid path

	Record* r = &_root;

	try {
		for (auto&& token : tokens) {
			auto type = r->getType();
			if (type != RecordType::kUndefined && type != RecordType::kAssocArray) {
				return nullptr;
			}
			r = &(*r)[token.c_str()];
		}
	} catch (...) {
		return nullptr;
	}
	return r;
}