#include "Datastore.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <vector>

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

std::vector<std::string> Datastore::_tokenizePath(const char* key) {
	const char* pos = key;

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

const char* Datastore::getValue(const char* key) {
	if (_good == false) {
		return "";
	}
	if (auto r = getRecord(key)) {
		return r->getVal().c_str();
	}
	return "";
}

Record* Datastore::getRecord(const char* key) {
	// TODO: string operations like this are expensive, optimize?
	auto tokens = _tokenizePath(key);

	Record* r = &_root;

	size_t depth = 0;
	size_t targetDepth = tokens.size();

	try {
		while (r->getType() != RecordType::kUndefined) {
			if (depth == targetDepth) {
				if (r->getType() == RecordType::kString) {
					return r;
				}
				return nullptr;
			}
			if (r->getType() == RecordType::kAssocArray) {
				r = &(*r)[tokens[depth].c_str()];
			} else {
				r = &(*r)[std::stoll(tokens[depth])];
			}
			++depth;
		}
		return nullptr;
	} catch (...) {
		return nullptr;
	}
}

Record* Datastore::createPath(const char* path) {
	// TODO: string operations like this are expensive, optimize?
	auto tokens = _tokenizePath(path);

	if (tokens.size() == 0) { return nullptr; } // invalid path

	Record* r = &_root;

	size_t depth = 0;
	size_t targetDepth = tokens.size();

	try {
		while (depth < targetDepth) {
			auto type = r->getType();
			if (type != RecordType::kUndefined && type != RecordType::kAssocArray) {
				return nullptr;
			}
			r = &(*r)[tokens[depth].c_str()];
			++depth;
		}
		return r;
	} catch (...) {
		return nullptr;
	}
}