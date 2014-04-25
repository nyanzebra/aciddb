#include "Datastore.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <vector>

Datastore::Datastore(std::istream& in) {
	boost::archive::binary_iarchive iarch(in);
	try {
		iarch >> _root;	
	} catch (...) {
		_good = false;
	}
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
			ret.push_back("");
		} else {
			ret.back().push_back(*pos);
		}
		++pos;
	}
	return ret;
}

const char* Datastore::getKey(const char* path) {
	if (_good == false) {
		return "";
	}
	if (auto r = _getRecord(path)) {
		return r->getVal().c_str();
	}
	return "";
}

Record* Datastore::_getRecord(const char* path) {
	// TODO: string operations like this are expensive, optimize?
	auto tokens = _tokenizePath(path);

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