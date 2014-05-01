#include "Datastore.h"

#include "../../shared/src/logging.h"

#include <vector>

static const std::string gEmptyString;

Datastore::Datastore(std::istream& in) {
	try {
		auto pos = in.tellg();

		in.seekg(0, in.end);

		if (in.tellg() - pos == 0) {
			// empty input stream
			return;
		}
		in.seekg(pos);

		DatastoreInputArchiveType iarch(in);
		iarch >> _root;
	} catch (std::exception& e) {
		Logf(kLogLevelInfo, "unable to parse datastore input stream");
		_good = false;
	}
}

bool Datastore::write(std::ostream& out) {
	if (!_good) {
		return false;
	}

	try {

		DatastoreOutputArchiveType oarch(out);
		oarch << _root;

		return true;
	} catch (...) {}
	return false;
}

std::vector<std::string> Datastore::_tokenizePath(const std::string& path) {
	if (path.empty()) { return {}; } // invalid path

	bool escaped = false;

	std::vector<std::string> ret = {""};

	for (auto&& c : path) {
		if (escaped) {
			switch (c) {
				case '\\':
				case ':':
					ret.back().push_back(c);
			}
			escaped = false;
		} else if (c == '\\') {
			escaped = true;
		} else if (c == ':') {
			if (ret.back().empty()) {
				return {}; // no part of the path should be empty
			}
			ret.push_back("");
		} else {
			ret.back().push_back(c);
		}
	}
	// no part of the path should be empty
	if (ret.size() == 1 && ret.back() == "") {
		return {};
	}
	return ret;
}

void Datastore::removeRecord(const std::string& path) {

	auto tokens = _tokenizePath(path);
	if (tokens.empty()) { return; } // invalid path

	auto records = _recordsOnPath(tokens);
	if (records.empty()) { return; }
	if (!records.back()) { return; }

	// records array now has no null pointers

	// records and tokens correlate 1:1.
	// _root -> records[0] -> records[1] -> ...
	//      tokens[0] -> tokens[1] -> ...
	// The associations between parent and child are named.
	// The names are the tokens.

	records.pop_back();
	Record* parent = records.empty() ? &_root : records.back();
	parent->removeChild(tokens.back().c_str());
	tokens.pop_back();

	try {
		while (tokens.size() > 0) {
			if (parent->numChildren() == 0) {
				records.pop_back();
				parent = records.empty() ? &_root : records.back();
				parent->removeChild(tokens.back().c_str());
				tokens.pop_back();
			} else {
				break;
			}
		}
	} catch (std::exception& e) {
		Logf("%s", e.what());
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
			ret.insert(ret.end(), path.size() - ret.size(), nullptr); // fill remainder with null
			return ret;
		}
		ret.push_back(r);
	}
	return ret;
}

const std::string& Datastore::getValue(const std::string& key) {
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

Record* Datastore::getRecord(const std::string& path) {

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

Record* Datastore::createPath(const std::string& path) {

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